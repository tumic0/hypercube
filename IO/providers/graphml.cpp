#include <cstring>
#include <cerrno>
#include "IO/encodings/utf8cvt.h"
#include "graphml.h"

using namespace std;


#define isStartName(c) \
	(isalnum((c)) || (c) == '_' || (c) == ':')
#define isName(c) \
	(isStartName((c)) || (c) == '-' || (c) == '.')


#define NUM_RELATIONS (sizeof(relations) / sizeof(Relation))
const GraphmlGraphInput::Relation GraphmlGraphInput::relations[] = {
	{L"graphml", L""},
	{L"graph", L"graphml"},
	{L"node", L"graph"},
	{L"edge", L"graph"}
};


Vertex* GraphmlGraphInput::addVertex(const wstring &id)
{
	Vertex *v;
	map<wstring, Vertex*>::const_iterator it;

	it = _vertexes.find(id);
	if (it != _vertexes.end())
		return it->second;

	v = _graph->addVertex();

	_vertexes.insert(pair<wstring, Vertex*>(id, v));

	return v;
}

Edge* GraphmlGraphInput::addEdge(const wstring &source, const wstring &target)
{
	Vertex *src, *dst;

	src = addVertex(source);
	dst = addVertex(target);

	return _graph->addEdge(src, dst);
}

void GraphmlGraphInput::checkRelation(const wstring &node, const wstring &parent)
{
	for (size_t i = 0; i < NUM_RELATIONS; i++) {
		if (node == relations[i].node) {
			if (parent != relations[i].parent)
				error();
			return;
		}
	}
}


void GraphmlGraphInput::error()
{
	if (_token == ERROR)
		return;

	ioerr << "GraphML: parse error on line: " << _line << endl;
	_token = ERROR;
}

void GraphmlGraphInput::nextToken()
{
	int c, state = 0;


	while (1) {
		c = _fs.get();

		if (!_fs.good())
			c = -1;

		switch (state) {
			case 0:
				if (isspace(c)) {
					if (c == '\n')
						_line++;
					break;
				}
				if (c == '<') {
					state = 3;
					break;
				}
				if (c == '>') {
					_token = GT;
					return;
				}
				if (c == '=') {
					_token = EQ;
					return;
				}
				if (c == '&') {
					_token = AMP;
					return;
				}
				if (c == '/') {
					_token = SLASH;
					return;
				}
				if (c == '?') {
					_token = QM;
					return;
				}
				if (isStartName(c)) {
					_string = c;
					state = 1;
					break;
				}
				if (c == '"') {
					_string.clear();
					state = 2;
					break;
				}
				if (c == -1) {
					_token = EOI;
					return;
				}
				error();
				return;

			case 1:
				if (isName(c)) {
					_string += c;
					break;
				}
				_fs.putback(c);
				_token = IDENT;
				return;

			case 2:
				if (c == '"') {
					_token = STRING;
					return;
				}
				if (c == '\n')
					_line++;
				_string += c;
				break;

			case 3:
				if (c == '!') {
					state = 4;
					break;
				}
				_fs.putback(c);
				_token = LT;
				return;

			case 4:
				if (c == '-') {
					state = 5;
					break;
				}
				error();

			case 5:
				if (c == '-') {
					state = 6;
					break;
				}
				error();

			case 6:
				if (c == '-')
					state = 7;
				if (c == '\n')
					_line++;
				break;

			case 7:
				if (c == '-')
					state = 8;
				else
					state = 6;
				break;

			case 8:
				if (c == '>')
					state = 0;
				else
					state = 6;
				break;
		}
	}
}

void GraphmlGraphInput::compare(Token token)
{
	if (_token == token)
		nextToken();
	else
		error();
}

void GraphmlGraphInput::data()
{
	while (1) {
		switch (_token) {
			case LT:
			case ERROR:
			case EOI:
				return;
			default:
				nextToken();
		}
	}
}

void GraphmlGraphInput::attribute()
{
	wstring attr, value;

	attr = _string;
	compare(IDENT);
	compare(EQ);
	value = _string;
	compare(STRING);

	if (_token == ERROR)
		return;

	if (attr == L"id")
		_attributes.id = value;
	if (attr == L"source")
		_attributes.source = value;
	if (attr == L"target")
		_attributes.target = value;
}

bool GraphmlGraphInput::attributes()
{
	while (1) {
		switch (_token) {
			case SLASH:
				nextToken();
				return true;
			case GT:
				return false;
			case IDENT:
				attribute();
				break;
			default:
				error();
				return false;
		}
	}
}

void GraphmlGraphInput::elementType(const wstring &parent)
{
	switch (_token) {
		case SLASH:
			break;
		case IDENT:
			element(parent);
			break;
		default:
			error();
	}
}

void GraphmlGraphInput::nextItem(const wstring &parent)
{
	switch (_token) {
		case LT:
			nextToken();
			elementType(parent);
			break;
		case IDENT:
			data();
			compare(LT);
			break;
		case EOI:
			break;
		default:
			error();
	}
}

void GraphmlGraphInput::element(const wstring &parent)
{
	bool closed;
	wstring start, end;
	Vertex *vertex;
	Edge *edge;


	start = _string;
	compare(IDENT);
	checkRelation(start, parent);
	closed = attributes();
	compare(GT);

	if (_token == ERROR)
		return;

	if (start == L"node") {
		vertex = addVertex(_attributes.id);
		vertex->setText(_attributes.id);
	}
	if (start == L"edge") {
		edge = addEdge(_attributes.source, _attributes.target);
		edge->setText(_attributes.id);
	}

	if (closed)
		return;

	while (_token == LT || _token == IDENT)
		nextItem(start);

	compare(SLASH);
	end = _string;
	compare(IDENT);
	compare(GT);

	if (start != end)
		error();
}

void GraphmlGraphInput::xmlAttributes()
{
	while (1) {
		switch (_token) {
			case QM:
			case ERROR:
			case EOI:
				return;
			default:
				nextToken();
		}
	}
}

void GraphmlGraphInput::xmlProlog()
{
	wstring elm;

	nextToken();
	elm = _string;
	compare(IDENT);
	xmlAttributes();
	compare(QM);
	compare(GT);

	if (elm != L"xml")
		error();
}

void GraphmlGraphInput::xml()
{
	compare(LT);

	switch (_token) {
		case QM:
			xmlProlog();
			compare(LT);
			element(L"");
			break;
		case IDENT:
			element(L"");
			break;
		default:
			error();
	}
}

bool GraphmlGraphInput::parse()
{
	_line = 1;
	_token = START;

	nextToken();
	xml();

	_vertexes.clear();

	if (_token == EOI)
		return true;
	else {
		error();
		return false;
	}
}

IO::Error GraphmlGraphInput::readGraph(Graph *graph, const char *fileName,
  Encoding *encoding)
{
	IO::Error err = Ok;

	_graph = graph;

	locale lc(std::locale(), new utf8cvt);
	_fs.imbue(lc);

	_fs.open(fileName);
	if (!_fs) {
		ioerr << fileName << ": " << strerror(errno) << endl;
		err = OpenError;
	} else {
		if (!parse())
			err = (_fs.fail()) ? ReadError : FormatError;
	}

	_fs.close();
	_fs.clear();

	if (err)
		_graph->clear();

	return err;
}
