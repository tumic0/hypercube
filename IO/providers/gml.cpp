#include <cstring>
#include <cerrno>
#include <sstream>
#include "CORE/misc.h"
#include "gml.h"

using namespace std;


#define ROOT     L""
#define GRAPH    L"graph"
#define NODE     L"node"
#define EDGE     L"edge"
#define TARGET   L"target"
#define SOURCE   L"source"
#define DIRECTED L"directed"
#define ID       L"id"


const GmlGraphInput::Relation GmlGraphInput::relations[] = {
	{NODE, GRAPH},
	{EDGE, GRAPH},
	{SOURCE, EDGE},
	{TARGET, EDGE},
	{GRAPH, ROOT},
	{DIRECTED, GRAPH}
};


void GmlGraphInput::error()
{
	if (_token == ERROR)
		return;

	ioerr << "GML: parse error on line: " << _line << endl;
	_token = ERROR;
}

void GmlGraphInput::nextToken()
{
	int c, state = 0;
	wstring flstr;
	bool negative = false;


	while (1) {
		c = _fs.get();

		switch (state) {
			case 0:
				if (isspace(c))
					break;
				if (c == '#') {
					state = 1;
					break;
				}
				if (c == '[') {
					_token = LBRK;
					return;
				}
				if (c == ']') {
					_token = RBRK;
					return;
				}
				if (isalpha(c)) {
					_string = c;
					state = 2;
					break;
				}
				if (isdigit(c)) {
					_int = c - '0';
					flstr += c;
					state = 3;
					break;
				}
				if (c == '.') {
					_int = 0;
					flstr += c;
					state = 4;
				}
				if (c == '+') {
					_int = 0;
					flstr += c;
					state = 3;
					break;
				}
				if (c == '-') {
					_int = 0;
					negative = true;
					flstr += c;
					state = 3;
					break;
				}
				if (c == '"') {
					_string.clear();
					state = 7;
					break;
				}
				if (c == EOF) {
					_token = EOI;
					return;
				}
				error();
				return;

			case 1:
				if (c == EOF) {
					_token = EOI;
					return;
				}
				if (c == '\n') {
					_line++;
					state = 0;
				}
				break;

			case 2:
				if (isalnum(c)) {
					_string += c;
					break;
				}
				_fs.unget();
				_token = KEY;
				return;

			case 3:
				if (c == '.') {
					flstr += c;
					state = 4;
					break;
				}
				if (c == 'e' || c == 'E') {
					flstr += c;
					state = 5;
					break;
				}
				if (isdigit(c)) {
					flstr += c;
					_int = _int * 10 + c - '0';
					break;
				}
				_fs.unget();
				if (negative)
					_int = -_int;
				_token = INT;
				return;

			case 4:
				if (isdigit(c)) {
					_int = _int * 10 + c - '0';
					flstr += c;
					break;
				}
				if (c == 'e' || c == 'E') {
					flstr += c;
					state = 5;
					break;
				}
				_fs.unget();
				wistringstream(flstr) >> _float;
				_token = REAL;
				return;

			case 5:
				if (c == '+') {
					flstr += c;
					state = 6;
					break;
				}
				if (c == '-') {
					flstr += c;
					state = 6;
					break;
				}
				if (isdigit(c)) {
					flstr += c;
					state = 6;
					break;
				}
				error();
				return;

			case 6:
				if (isdigit(c)) {
					flstr += c;
					break;
				}
				_fs.unget();
				wistringstream(flstr) >> _float;
				_token = REAL;
				return;

			case 7:
				if (c == EOF) {
					error();
					return;
				}
				if (c == '"') {
					_token = STRING;
					return;
				}
				_string += c;
				break;
		}
	}
}

void GmlGraphInput::compare(Token token)
{
	if (_token == token)
		nextToken();
	else
		error();
}

void GmlGraphInput::value(const wstring &parent, const wstring &key)
{
	switch (_token) {
		case INT:
			setIntAttribute(parent, key, _int);
			nextToken();
			break;
		case REAL:
			nextToken();
			break;
		case STRING:
			setStringAttribute(parent, key, _string);
			nextToken();
			break;
		case LBRK:
			nextToken();
			list(key);
			compare(RBRK);
			break;
		default:
			error();
	}
}

void GmlGraphInput::list(const wstring &parent)
{
	wstring key;

	while (1) {
		switch (_token) {
			case KEY:
				key = _string;
				if (!checkRelation(key, parent)) {
					error();
					return;
				}
				nextToken();
				value(parent, key);
				if (!handleKey(key))
					error();
				break;
			case RBRK:
			case EOI:
				return;
			default:
				error();
				return;
		}
	}
}

bool GmlGraphInput::parse()
{
	_line = 1;
	_token = START;

	clearNodeAttributes();
	clearEdgeAttributes();
	initGraphAttributes();

	nextToken();
	list(L"");

	_vertexes.clear();

	if (_token == EOI)
		return true;
	else {
		error();
		return false;
	}
}


bool GmlGraphInput::checkRelation(const wstring &node, const wstring &parent)
{
	for (size_t i = 0; i < ARRAY_SIZE(relations); i++) {
		if (node == relations[i].node) {
			if (parent != relations[i].parent)
				return false;
		}
	}

	return true;
}

bool GmlGraphInput::handleKey(const wstring &key)
{
	if (key == NODE) {
		Vertex *v;

		if (!(v = addVertex(_nodeAttributes.id)))
			return false;
		setVertexAttributes(v);

		clearNodeAttributes();
	} else if (key == EDGE) {
		Edge *e;

		if (!(e = addEdge(_edgeAttributes.source, _edgeAttributes.target)))
			return false;
		setEdgeAttributes(e);

		clearEdgeAttributes();
	}

	return true;
}

Vertex* GmlGraphInput::addVertex(int id)
{
	Vertex *v;
	map<int, Vertex*>::const_iterator it;

	if (id < 0)
		return 0;

	it = _vertexes.find(id);
	if (it != _vertexes.end())
		return it->second;

	v = _graph->addVertex();

	_vertexes.insert(pair<int, Vertex*>(id, v));

	return v;
}

Edge* GmlGraphInput::addEdge(int source, int target)
{
	Vertex *src, *dst;

	if (source < 0 || target < 0 || source == target)
		return 0;

	src = addVertex(source);
	dst = addVertex(target);

	return _graph->addEdge(src, dst);
}

void GmlGraphInput::clearNodeAttributes()
{
	_nodeAttributes.label.clear();
	_nodeAttributes.id = -1;
}

void GmlGraphInput::clearEdgeAttributes()
{
	_edgeAttributes.label.clear();
	_edgeAttributes.source = -1;
	_edgeAttributes.target = -1;
}

void GmlGraphInput::initGraphAttributes()
{
	_graphAttributes.directed = 0;
}

void GmlGraphInput::setIntAttribute(const wstring &parent, const wstring &key,
  int value)
{
	if (parent == NODE) {
		if (key == ID)
			_nodeAttributes.id = value;
	} else if (parent == EDGE) {
		if (key == SOURCE)
			_edgeAttributes.source = value;
		if (key == TARGET)
			_edgeAttributes.target = value;
	} else if (parent == GRAPH) {
		if (key == DIRECTED)
			_graphAttributes.directed = value ? true : false;
	}
}

void GmlGraphInput::setStringAttribute(const wstring &parent,
  const wstring &key, const wstring &value)
{
	if (parent == NODE && key == _nodeLabelAttr)
		_nodeAttributes.label = value;
	else if (parent == EDGE && key == _edgeLabelAttr)
		_edgeAttributes.label = value;
}

void GmlGraphInput::setVertexAttributes(Vertex *vertex)
{
	if (_nodeAttributes.label.empty()) {
		wostringstream ss;
		ss << _nodeAttributes.id;
		vertex->setText(ss.str());
	} else
		vertex->setText(_nodeAttributes.label);
}

void GmlGraphInput::setEdgeAttributes(Edge *edge)
{
	edge->setText(_edgeAttributes.label);
}


void GmlGraphInput::setInputEncoding(Encoding *encoding)
{
	_encoding = encoding;
}

void GmlGraphInput::setNodeLabelAttribute(const char *name)
{
	_nodeLabelAttr = s2w(name);
}

void GmlGraphInput::setEdgeLabelAttribute(const char *name)
{
	_edgeLabelAttr = s2w(name);
}

IO::Error GmlGraphInput::readGraph(Graph *graph, const char *fileName)
{
	IO::Error err = Ok;

	_graph = graph;

	if (_encoding) {
		locale lc(locale(), _encoding->cvt());
		_fs.imbue(lc);
	}

	_fs.open(fileName);
	if (!_fs) {
		ioerr << fileName << ": " << strerror(errno) << endl;
		err = OpenError;
	} else {
		if (!parse())
			err = (_fs.fail()) ? ReadError : FormatError;
		else
			_graph->setDirected(_graphAttributes.directed);
	}

	_fs.close();
	_fs.clear();

	if (err)
		_graph->clear();

	return err;
}
