#include <cstring>
#include <cerrno>
#include <istream>
#include "gml.h"

using namespace std;


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

		if (!_fs.good())
			c = -1;

		switch (state) {
			case 0:
				if (c == '#') {
					state = 1;
					break;
				}
				if (isspace(c)) {
					if (c == '\n')
						_line++;
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
				_fs.putback(c);
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
				_fs.putback(c);
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
				_fs.putback(c);
				std::wistringstream(flstr) >> _float;
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
				_fs.putback(c);
				std::wistringstream(flstr) >> _float;
				_token = REAL;
				return;

			case 7:
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
				if (((type == EDGE || type == NODE) && parent != GRAPH)
				  || ((type == SOURCE || type == TARGET) && parent != EDGE)) {
					error();
					return;
				}

				nextToken();
				value(parent, type);

				if (type == NODE) {
					if (_nodeAttributes.id < 0) {
						error();
						return;
					}
					Vertex *v = addVertex(_nodeAttributes.id);
					setVertexAttributes(v);
					attributesClear();
				}
				if (type == EDGE) {
					if (_edgeAttributes.source < 0
					  || _edgeAttributes.target < 0) {
						error();
						return;
					}
					Edge *e = addEdge();
					setEdgeAttributes(e);
					attributesClear();
				}

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
	attributesClear();

	nextToken();
	list(UNKNOWN);

	_vertexes.clear();
	attributesClear();

	if (_token == EOI)
		return true;
	else {
		error();
		return false;
	}
}

GmlGraphInput::ValueType GmlGraphInput::valueType()
{
	if (_string == L"graph")
		return GRAPH;
	if (_string == L"node")
		return NODE;
	if (_string == L"edge")
		return EDGE;
	if (_string == L"id")
		return ID;
	if (_string == L"label")
		return LABEL;
	if (_string == L"source")
		return SOURCE;
	if (_string == L"target")
		return TARGET;

	return UNKNOWN;
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

Edge* GmlGraphInput::addEdge()
{
	Vertex *src, *dst;

	src = addVertex(_edgeAttributes.source);
	dst = addVertex(_edgeAttributes.target);

	return _graph->addEdge(src, dst);
}

void GmlGraphInput::attributesClear()
{
	_nodeAttributes.label.clear();
	_nodeAttributes.id = -1;

	_edgeAttributes.label.clear();
	_edgeAttributes.source = -1;
	_edgeAttributes.target = -1;
}

void GmlGraphInput::setVertexAttributes(Vertex *vertex)
{
	if (_nodeAttributes.label.empty()) {
		std::wostringstream ss;
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

	if (encoding) {
		locale lc(std::locale(), encoding->cvt());
		_fs.imbue(lc);
	}

	_fs.open(fileName);
	if (!_fs) {
		ioerr << fileName << ": " << strerror(errno) << endl;
		return OpenError;
	}

	_graph = graph;

	if (!parse()) {
		err = FormatError;
		_graph->clear();
	}

	_fs.close();
	if (_fs.bad())
		err = ReadError;

	return err;
}
