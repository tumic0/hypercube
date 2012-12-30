#include <cstring>
#include <cerrno>
#include "IO/modules.h"
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


static std::wstring s2w(const string &s)
{
	std::wstring w(s.length(), L' ');
	std::copy(s.begin(), s.end(), w.begin());

	return w;
}


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

void GraphmlGraphInput::setEncoding(const wstring &encoding)
{
	codecvt<wchar_t,char,mbstate_t> *cvt = 0;

	for (Encoding **ep = encodings; *ep; ep++) {
		if (stringCaseCmp(encoding, s2w((*ep)->name()))) {
			cvt = (*ep)->cvt();
			break;
		}
	}
	if (!cvt) {
		cerr << "Unsupported encoding. Using UTF-8." << endl;
		cvt = new utf8cvt;
	}

	locale lc(std::locale(), cvt);
	_fs.imbue(lc);
}

void GraphmlGraphInput::setAttribute(const wstring &attr, const wstring &value)
{
	if (attr == L"id")
		_attributes.id = value;
	if (attr == L"source")
		_attributes.source = value;
	if (attr == L"target")
		_attributes.target = value;
	if (attr == L"encoding")
		_attributes.encoding = value;
}

void GraphmlGraphInput::clearAttributes()
{
	_attributes.id.clear();
	_attributes.source.clear();
	_attributes.target.clear();
}

void GraphmlGraphInput::handleElement(const wstring &element)
{
	Vertex *vertex;
	Edge *edge;

	if (element == L"node") {
		if (_attributes.id.empty()) {
			error();
			return;
		}
		vertex = addVertex(_attributes.id);
		vertex->setText(_attributes.id);
	}
	if (element == L"edge") {
		if (_attributes.source.empty() || _attributes.target.empty()) {
			error();
			return;
		}
		edge = addEdge(_attributes.source, _attributes.target);
		if (!_attributes.id.empty())
			edge->setText(_attributes.id);
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
					_token = LT;
					return;
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
				if (c == '!') {
					_token = EXCL;
					return;
				}
				if (c == '-') {
					_token = MINUS;
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
				_token = DATA;
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
				return;
			case EOI:
				error();
				return;
			default:
				nextToken();
		}
	}
}

void GraphmlGraphInput::commentData()
{
	while (1) {
		switch (_token) {
			case MINUS:
				nextToken();
				if (_token == MINUS)
					return;
				break;
			case ERROR:
				return;
			case EOI:
				error();
				return;
			default:
				nextToken();
		}
	}
}

void GraphmlGraphInput::comment()
{
	compare(EXCL);
	compare(MINUS);
	compare(MINUS);
	commentData();
	compare(MINUS);
	compare(GT);
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

	setAttribute(attr, value);
}

bool GraphmlGraphInput::attributes()
{
	clearAttributes();

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
		case EXCL:
			comment();
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
		case EOI:
		case ERROR:
			break;
		default:
			data();
			compare(LT);
			break;
	}
}

void GraphmlGraphInput::element(const wstring &parent)
{
	bool closed;
	wstring start, end;


	start = _string;
	compare(IDENT);
	checkRelation(start, parent);
	closed = attributes();
	compare(GT);
	if (_token == ERROR)
		return;

	handleElement(start);

	if (closed)
		return;

	while (_token != ERROR && _token != SLASH)
		nextItem(start);

	compare(SLASH);
	end = _string;
	compare(IDENT);
	compare(GT);

	if (start != end)
		error();
}

void GraphmlGraphInput::rootElement()
{
	switch (_token) {
		case EXCL:
			comment();
			compare(LT);
			rootElement();
			break;
		case IDENT:
			setEncoding(_attributes.encoding);
			element(L"");
			break;
		default:
			error();
	}
}

void GraphmlGraphInput::xmlAttributes()
{
	while (1) {
		switch (_token) {
			case QM:
			case ERROR:
				return;
			default:
				attribute();
		}
	}
}

void GraphmlGraphInput::xmlProlog()
{
	nextToken();
	if (_string != L"xml")
		error();
	compare(IDENT);
	xmlAttributes();
	compare(QM);
	compare(GT);
}

void GraphmlGraphInput::xml()
{
	compare(LT);

	switch (_token) {
		case QM:
			xmlProlog();
			compare(LT);
			rootElement();
			break;
		case IDENT:
		case EXCL:
			rootElement();
			break;
		default:
			error();
	}
}

bool GraphmlGraphInput::parse()
{
	_line = 1;
	_token = START;
	_attributes.encoding = L"utf-8";

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
