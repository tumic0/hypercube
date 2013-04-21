#include <cstring>
#include <cerrno>
#include "CORE/misc.h"
#include "IO/modules.h"
#include "IO/encodings/utf8cvt.h"
#include "IO/encodings/latin1.h"
#include "graphml.h"

using namespace std;


#define isStartName(c) \
	(isalnum((c)) || (c) == '_' || (c) == ':')
#define isName(c) \
	(isStartName((c)) || (c) == '-' || (c) == '.')


const unsigned char BOM[] = {0xEF, 0xBB, 0xBF};

const GraphmlGraphInput::Relation GraphmlGraphInput::relations[] = {
	{L"graphml", L""},
	{L"graph", L"graphml"},
	{L"node", L"graph"},
	{L"edge", L"graph"}
};


static std::wstring s2w(const string &s)
{
	wstring w(s.length(), L' ');
	copy(s.begin(), s.end(), w.begin());

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
	if (parent.empty()) {
		if (node != L"graphml") {
			error();
			return;
		}
	}

	for (size_t i = 0; i < ARRAY_SIZE(relations); i++) {
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

void GraphmlGraphInput::setAttribute(const wstring &element,
  const wstring &attr, const wstring &value)
{
	if (element == L"node") {
		if (attr == L"id")
			_nodeAttributes.id = value;
	} else if (element == L"edge") {
		if (attr == L"id")
			_edgeAttributes.id = value;
		if (attr == L"source")
			_edgeAttributes.source = value;
		if (attr == L"target")
			_edgeAttributes.target = value;
	} else if (element == L"xml") {
		if (attr == L"encoding")
			_graphAttributes.encoding = value;
	} else if (element == L"graph") {
		if (attr == L"edgedefault")
			_graphAttributes.edgedefault = value;
	}
}

void GraphmlGraphInput::initGraphAttributes()
{
	_graphAttributes.encoding = L"utf-8";
	_graphAttributes.edgedefault = L"directed";
}

void GraphmlGraphInput::clearNodeAttributes()
{
	_nodeAttributes.id.clear();
}

void GraphmlGraphInput::clearEdgeAttributes()
{
	_edgeAttributes.id.clear();
	_edgeAttributes.source.clear();
	_edgeAttributes.target.clear();
}

void GraphmlGraphInput::handleElement(const wstring &element)
{
	Vertex *vertex;
	Edge *edge;

	if (element == L"node") {
		if (_nodeAttributes.id.empty()) {
			error();
			return;
		}
		vertex = addVertex(_nodeAttributes.id);
		vertex->setText(_nodeAttributes.id);

		clearNodeAttributes();
	}
	if (element == L"edge") {
		if (_edgeAttributes.source.empty() || _edgeAttributes.target.empty()) {
			error();
			return;
		}
		edge = addEdge(_edgeAttributes.source, _edgeAttributes.target);
		if (!_edgeAttributes.id.empty())
			edge->setText(_edgeAttributes.id);

		clearEdgeAttributes();
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
				if (c == '[') {
					_token = LSB;
					return;
				}
				if (c == ']') {
					_token = RSB;
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
				if (c == '\'') {
					_string.clear();
					state = 3;
					break;
				}
				if (c == EOF) {
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
				_fs.unget();
				_token = IDENT;
				return;

			case 2:
				if (c == EOF) {
					error();
					return;
				}
				if (c == '"') {
					_token = STRING;
					return;
				}
				if (c == '\n')
					_line++;
				_string += c;
				break;

			case 3:
				if (c == EOF) {
					error();
					return;
				}
				if (c == '\'') {
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
	int c;

	while (1) {
		c = _fs.get();

		if (c == '<') {
			_token = LT;
			return;
		} else if (c == EOF) {
			_token = EOI;
			return;
		} else if (c == '\n')
			_line++;
	}
}

void GraphmlGraphInput::dtdElement()
{
	while (1) {
		switch (_token) {
			case GT:
				nextToken();
				return;
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

void GraphmlGraphInput::dtdElementType()
{
	switch (_token) {
		case MINUS:
			comment();
			break;
		case IDENT:
			dtdElement();
			break;
		default:
			error();
	}
}

void GraphmlGraphInput::dtdData()
{
	while (1) {
		switch (_token) {
			case LT:
				nextToken();
				compare(EXCL);
				dtdElementType();
				break;
			case RSB:
				nextToken();
				return;
			default:
				error();
				return;
		}
	}
}

void GraphmlGraphInput::cdataData()
{
	while (1) {
		switch (_token) {
			case RSB:
				nextToken();
				if (_token == RSB) {
					nextToken();
					if (_token == GT)
						return;
				}
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

void GraphmlGraphInput::commentData()
{
	int c, state = 0;

	while (1) {
		c = _fs.get();

		if (c == '\n')
			_line++;

		switch (state) {
			case 0:
				if (c == EOF) {
					error();
					return;
				}
				if (c == '-')
					state = 1;
				break;

			case 1:
				if (c == EOF) {
					error();
					return;
				}
				if (c == '-')
					state = 2;
				else
					state = 0;
				break;

			case 2:
				if (c == '>') {
					_token = GT;
					return;
				}
				error();
				return;
		}
	}
}

void GraphmlGraphInput::comment()
{
	compare(MINUS);
	if (_token != MINUS) {
		error();
		return;
	}
	commentData();
	compare(GT);
}

void GraphmlGraphInput::cdata()
{
	compare(LSB);
	compare(IDENT);
	compare(LSB);
	cdataData();
	compare(GT);
}

void GraphmlGraphInput::special()
{
	switch (_token) {
		case MINUS:
			comment();
			break;
		case LSB:
			cdata();
			break;
		default:
			error();
	}
}

void GraphmlGraphInput::attribute(const wstring &element)
{
	wstring attr, value;

	attr = _string;
	compare(IDENT);
	compare(EQ);
	value = _string;
	compare(STRING);

	if (_token == ERROR)
		return;

	setAttribute(element, attr, value);
}

void GraphmlGraphInput::xmlAttributes()
{
	while (1) {
		switch (_token) {
			case QM:
			case ERROR:
				return;
			default:
				attribute(L"xml");
		}
	}
}

bool GraphmlGraphInput::attributes(const wstring &element)
{
	while (1) {
		switch (_token) {
			case SLASH:
				nextToken();
				return true;
			case GT:
				return false;
			case IDENT:
				attribute(element);
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
			nextToken();
			special();
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
		default:
			error();
	}
}

void GraphmlGraphInput::elementEnd()
{
	if (_token != GT) {
		error();
		return;
	}
	data();
}

void GraphmlGraphInput::element(const wstring &parent)
{
	bool closed;
	wstring start, end;


	start = _string;
	compare(IDENT);
	checkRelation(start, parent);
	closed = attributes(start);
	elementEnd();
	handleElement(start);

	if (closed)
		return;

	do {
		nextItem(start);
	} while (_token != ERROR && _token != SLASH);

	compare(SLASH);
	end = _string;
	compare(IDENT);
	elementEnd();

	if (start != end)
		error();
}

void GraphmlGraphInput::dtdMarkupDecl()
{
	switch (_token) {
		case LSB:
			nextToken();
			dtdData();
			break;
		case GT:
			return;
		default:
			error();
	}
}

void GraphmlGraphInput::dtdExternalId()
{
	wstring type = _string;

	switch (_token) {
		case LSB:
			break;
		case IDENT:
			nextToken();
			if (type == L"PUBLIC") {
				compare(STRING);
				compare(STRING);
			} else if (type == L"SYSTEM")
				compare(STRING);
			else
				error();
			break;
		default:
			error();
	}
}

void GraphmlGraphInput::dtdDecl()
{
	if (_string != L"DOCTYPE")
		error();
	compare(IDENT);
	compare(IDENT);
	dtdExternalId();
	dtdMarkupDecl();
	compare(GT);
}

void GraphmlGraphInput::xmlDecl()
{
	compare(QM);
	if (!stringCaseCmp(_string, L"xml"))
		error();
	compare(IDENT);
	xmlAttributes();
	compare(QM);
	compare(GT);
}

void GraphmlGraphInput::prologComment()
{
	switch (_token) {
		case EXCL:
			nextToken();
			comment();
			compare(LT);
			prologComment();
			break;
		case IDENT:
			break;
		default:
			error();
	}
}

void GraphmlGraphInput::prologSpecial()
{
	switch (_token) {
		case MINUS:
			comment();
			compare(LT);
			prologContent();
			break;
		case IDENT:
			dtdDecl();
			compare(LT);
			prologComment();
			break;
		default:
			error();
	}
}

void GraphmlGraphInput::prologContent()
{
	switch (_token) {
		case EXCL:
			nextToken();
			prologSpecial();
			break;
		case IDENT:
			break;
		default:
			error();
	}
}

void GraphmlGraphInput::xml()
{
	compare(LT);

	switch (_token) {
		case QM:
			xmlDecl();
			compare(LT);
			prologContent();
			break;
		case EXCL:
			nextToken();
			prologSpecial();
			break;
		case IDENT:
			break;
		default:
			error();
	}

	setEncoding(_graphAttributes.encoding);
	element(L"");
}

void GraphmlGraphInput::bom()
{
	unsigned char bom[sizeof(BOM)];

	if (_fs.peek() == BOM[0]) {
		for (size_t i = 0; i < sizeof(BOM); i++)
			bom[i] = _fs.get();
		if (memcmp(bom, BOM, sizeof(BOM))) {
			error();
			return;
		}
	}

	nextToken();
}

bool GraphmlGraphInput::parse()
{
	_line = 1;
	_token = START;

	initGraphAttributes();

	bom();
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
  Encoding *)
{
	IO::Error err = Ok;

	_graph = graph;

	locale lc(std::locale(), new latin1cvt);
	_fs.imbue(lc);

	_fs.open(fileName);
	if (!_fs) {
		ioerr << fileName << ": " << strerror(errno) << endl;
		err = OpenError;
	} else {
		if (!parse())
			err = (_fs.fail()) ? ReadError : FormatError;
		else
			_graph->setDirected((_graphAttributes.edgedefault == L"directed"));
	}

	_fs.close();
	_fs.clear();

	if (err)
		_graph->clear();

	return err;
}
