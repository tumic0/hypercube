#include <cstring>
#include <cerrno>
#include <sstream>
#include "CORE/misc.h"
#include "gml.h"

using namespace std;


const GmlGraphInput::Keyword GmlGraphInput::keywords[] = {
	{NODE, L"node"},
	{EDGE, L"edge"},
	{GRAPH, L"graph"},
	{ID, L"id"},
	{LABEL, L"label"},
	{SOURCE, L"source"},
	{TARGET, L"target"},
	{DIRECTED, L"directed"}
};

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
				if (c == -1) {
					_token = EOI;
					return;
				}
				error();
				return;

			case 1:
				if (c == -1) {
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
				if (c == -1) {
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

void GmlGraphInput::value(ValueType parent, ValueType key)
{
	switch (_token) {
		case INT:
			if (parent == NODE && key == ID)
				_nodeAttributes.id = _int;
			if (parent == EDGE && key == SOURCE)
				_edgeAttributes.source = _int;
			if (parent == EDGE && key == TARGET)
				_edgeAttributes.target = _int;
			if (parent == GRAPH && key == DIRECTED)
				_graphAttributes.directed = _int ? true : false;
			nextToken();
			break;
		case REAL:
			nextToken();
			break;
		case STRING:
			if (parent == NODE && key == LABEL)
				_nodeAttributes.label = _string;
			if (parent == EDGE && key == LABEL)
				_edgeAttributes.label = _string;
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

void GmlGraphInput::list(ValueType parent)
{
	ValueType type = UNKNOWN;

	while (1) {
		switch (_token) {
			case KEY:
				type = valueType();
				checkRelation(type, parent);
				nextToken();
				value(parent, type);
				if (!handleKey(type))
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
	clearGraphAttributes();

	nextToken();
	list(ROOT);

	_vertexes.clear();

	if (_token == EOI)
		return true;
	else {
		error();
		return false;
	}
}

GmlGraphInput::ValueType GmlGraphInput::valueType()
{
	for (size_t i = 0; i < ARRAY_SIZE(keywords) ; i++)
		if (_string == keywords[i].name)
			return keywords[i].value;

	return UNKNOWN;
}

void GmlGraphInput::checkRelation(ValueType key, ValueType parent)
{
	for (size_t i = 0; i < ARRAY_SIZE(relations); i++) {
		if (key == relations[i].key) {
			if (parent != relations[i].parent)
				error();
			return;
		}
	}
}

bool GmlGraphInput::handleKey(ValueType type)
{
	Vertex *v;
	Edge *e ;

	switch (type) {
		case NODE:
			if (_nodeAttributes.id < 0)
				return false;
			v = addVertex(_nodeAttributes.id);
			setVertexAttributes(v);
			clearNodeAttributes();
			return true;

		case EDGE:
			if (_edgeAttributes.source < 0 || _edgeAttributes.target < 0)
				return false;
			e = addEdge(_edgeAttributes.source, _edgeAttributes.target);
			setEdgeAttributes(e);
			clearEdgeAttributes();
			return true;

		default:
			return true;
	}
}

Vertex* GmlGraphInput::addVertex(int id)
{
	Vertex *v;
	map<int, Vertex*>::const_iterator it;

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

void GmlGraphInput::clearGraphAttributes()
{
	_graphAttributes.directed = 0;
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


IO::Error GmlGraphInput::readGraph(Graph *graph, const char *fileName,
  Encoding *encoding)
{
	IO::Error err = Ok;

	_graph = graph;

	if (encoding) {
		locale lc(locale(), encoding->cvt());
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
