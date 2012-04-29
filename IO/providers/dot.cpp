#include <cstring>
#include <cerrno>
#include <cctype>
#include "dot.h"

using namespace std;


#define NUM_KEYWORDS (sizeof(keywords) / sizeof(Keyword))
const DotGraphInput::Keyword DotGraphInput::keywords[] = {
	{NODE, L"NODE"},
	{EDGE, L"EDGE"},
	{GRAPH, L"GRAPH"},
	{DIGRAPH, L"DIGRAPH"},
	{SUBGRAPH, L"SUBGRAPH"},
	{STRICT, L"STRICT"}
};


static bool strCaseCmp(const wstring &str1, const wstring &str2)
{
	if (str1.length() != str2.length())
		return false;

	for (size_t i = 0; i < str1.length(); i++)
		if (tolower(str1[i]) != tolower(str2[i]))
			return false;

	return true;
}


DotGraphInput::Token DotGraphInput::keyword()
{
	for (size_t i = 0; i < NUM_KEYWORDS ; i++)
		if (strCaseCmp(_id, keywords[i].name))
			return keywords[i].token;

	return ID;
}

void DotGraphInput::error()
{
	if (_token == ERROR)
		return;

	ioerr << "DOT: parse error on line: " << _line << endl;
	_token = ERROR;
}

void DotGraphInput::nextToken()
{
	int c, state = 0, parenthesis = 0;


	_id.clear();

	while (1) {
		c = _fs.get();

		if (!_fs.good()) {
			_token = (state) ? ERROR : EOI;
			return;
		}

		switch (state) {
			case 0:
				if (isspace(c)) {
					if (c == '\n')
						_line++;
					break;
				}
				if (c == '/') {
					state = 3;
					break;
				}

				if (c == ':') {
					_token = COLON;
					return;
				}
				if (c == ';') {
					_token = SEMICOLON;
					return;
				}
				if (c == ',') {
					_token = COMMA;
					return;
				}
				if (c == '=') {
					_token = EQ;
					return;
				}
				if (c == '{') {
					_token = LBRC;
					return;
				}
				if (c == '}') {
					_token = RBRC;
					return;
				}
				if (c == '[') {
					_token = LBRK;
					return;
				}
				if (c == ']') {
					_token = RBRK;
					return;
				}

				if (c == '-') {
					_id += c;
					state = 1;
					break;
				}
				if (c == '.') {
					_id += c;
					state = 9;
					break;
				}
				if (isdigit(c)) {
					_id += c;
					state = 11;
					break;
				}
				if (isalpha(c) || c == '_') {
					_id += c;
					state = 2;
					break;
				}
				if (c == '"') {
					state = 7;
					break;
				}
				if (c == '<') {
					state = 12;
					break;
				}

				error();
				return;

			case 1:
				if (c == '-' || c == '>') {
					_token = EDGEOP;
					return;
				}
				if (c == '.') {
					_id += c;
					state = 9;
					break;
				}
				if (isdigit(c)) {
					_id += c;
					state = 11;
					break;
				}

				error();
				return;

			case 2:
				if (isalnum(c) || c == '_') {
					_id += c;
					break;
				}
				_fs.unget();
				_token = keyword();
				return;

			case 3:
				if (c == '/') {
					state = 4;
					break;
				}
				if (c == '*') {
					state = 5;
					break;
				}

				error();
				return;

			case 4:
				if (c == '\n')
					state = 0;
				break;

			case 5:
				if (c == '*')
					state = 6;
				if (c == '\n')
					_line++;
				break;

			case 6:
				if (c == '/')
					state = 0;
				else
					state = 5;
				break;

			case 7:
				if (c == '"') {
					_token = ID;
					return;
				}
				if (c == '\\') {
					state = 8;
					break;
				}
				_id += c;
				break;

			case 8:
				if (c == '"')
					_id += '"';
				else {
					_id += '\\';
					_id += c;
				}
				state = 7;
				break;

			case 9:
				if (isdigit(c)) {
					state = 10;
					_id += c;
					break;
				}
				error();
				return;

			case 10:
				if (isdigit(c)) {
					_id += c;
					break;
				}
				_fs.unget();
				_token = ID;
				return;

			case 11:
				if (c == '.') {
					_id += c;
					state = 10;
					break;
				}
				if (isdigit(c)) {
					_id += c;
					break;
				}
				_fs.unget();
				_token = ID;
				return;

			case 12:
				if (c == '>') {
					if (!parenthesis) {
						_token = ID;
						return;
					} else
						parenthesis--;
				} else if (c == '<')
					parenthesis++;
				_id += c;
				break;
		}
	}
}

void DotGraphInput::compare(Token token)
{
	if (_token == token)
		nextToken();
	else
		error();
}


void DotGraphInput::list(Attributes &attr)
{
	wstring key(_id);

	compare(ID);

	if (_token == EQ) {
		nextToken();
		setAttribute(attr, key, _id);
		compare(ID);
	}

	if (_token == COMMA)
		nextToken();

	if (_token == ID)
		list(attr);
}

void DotGraphInput::attributeList(Attributes &attr)
{
	compare(LBRK);
	if (_token == ID)
		list(attr);
	compare(RBRK);
	if (_token == LBRK)
		attributeList(attr);
}

void DotGraphInput::edge(idSet &src, edgeSet &edges)
{
	idSet dst;
	Attributes attr;
	Vertex *v;
	Edge *e;

	nextToken();

	switch (_token) {
		case ID:
			v = addVertex(_id);
			setVertexAttributes(v, _nodeAttributes);
			dst.insert(_id);
			nodeId();
			break;
		case SUBGRAPH:
		case LBRC:
			subgraph(dst);
			break;
		default:
			error();
	}

	for (idSet::iterator i = src.begin(); i != src.end(); i++) {
		for (idSet::iterator j = dst.begin(); j != dst.end(); j++) {
			e = addEdge(*i, *j);
			setEdgeAttributes(e, _edgeAttributes);
			edges.insert(e);
		}
	}

	if (_token == EDGEOP)
		edge(dst, edges);
	if (_token == LBRK)
		attributeList(attr);

	for (edgeSet::iterator i = edges.begin(); i != edges.end(); i++)
		setEdgeAttributes(*i, attr);
}

void DotGraphInput::subgraphStatement(idSet &vertexes)
{
	edgeSet edges;

	subgraph(vertexes);

	if (_token == EDGEOP)
		edge(vertexes, edges);
}

void DotGraphInput::compassPt()
{
	if (_token == COLON) {
		nextToken();
		compare(ID);
	}
}

void DotGraphInput::nodeId()
{
	nextToken();

	if (_token == COLON) {
		nextToken();
		compare(ID);
		compassPt();
	}
}

void DotGraphInput::idStatement(idSet &subgraph)
{
	wstring vertex(_id);
	Attributes attr;
	idSet vertexes;
	edgeSet edges;
	Vertex *v;

	nodeId();

	switch (_token) {
		case EDGEOP:
			v = addVertex(vertex);
			setVertexAttributes(v, _nodeAttributes);
			subgraph.insert(vertex);
			vertexes.insert(vertex);
			edge(vertexes, edges);
			break;
		case EQ:
			nextToken();
			compare(ID);
			break;
		case LBRK:
			v = addVertex(vertex);
			setVertexAttributes(v, _nodeAttributes);
			subgraph.insert(vertex);
			attributeList(attr);
			setVertexAttributes(v, attr);
			break;
		case RBRC:
		case SEMICOLON:
			v = addVertex(vertex);
			setVertexAttributes(v, _nodeAttributes);
			subgraph.insert(vertex);
			break;
		default:
			error();
	}
}

void DotGraphInput::attributeStatement()
{
	Attributes attr;
	Token attributesType = _token;

	nextToken();
	attributeList(attr);

	if (attributesType == NODE)
		mergeAttributes(_nodeAttributes, attr);
	else if (attributesType == EDGE)
		mergeAttributes(_edgeAttributes, attr);
}

void DotGraphInput::subgraph(idSet &parent)
{
	idSet child;

	if (_token == SUBGRAPH) {
		nextToken();
		if (_token == ID)
			nextToken();
	}

	compare(LBRC);
	statementList(child);
	compare(RBRC);

	parent.insert(child.begin(), child.end());
}

void DotGraphInput::statementList(idSet &vertexes)
{
	while (1) {
		switch (_token) {
			case RBRC:
			case ERROR:
			case EOI:
				return;
			case LBRC:
			case SUBGRAPH:
				subgraphStatement(vertexes);
				break;
			case ID:
				idStatement(vertexes);
				break;
			case GRAPH:
			case NODE:
			case EDGE:
				attributeStatement();
				break;
			default:
				error();
		}

		if (_token == SEMICOLON)
			nextToken();
	}
}

void DotGraphInput::graphId()
{
	switch (_token) {
		case LBRC:
			break;
		case ID:
			nextToken();
			break;
		default:
			error();
	}
}

void DotGraphInput::graphType()
{
	switch (_token) {
		case GRAPH:
		case DIGRAPH:
			nextToken();
			break;
		default:
			error();
	}
}

void DotGraphInput::graph()
{
	idSet vertexes;

	if (_token == STRICT)
		nextToken();

	graphType();
	graphId();
	compare(LBRC);
	statementList(vertexes);
	compare(RBRC);
}

bool DotGraphInput::parse()
{
	_line = 1;
	_token = START;

	nextToken();
	graph();

	return (_token == EOI) ? 1 : 0;
}


void DotGraphInput::attributesClear()
{
	_nodeAttributes.label.clear();
	_edgeAttributes.label.clear();
}

void DotGraphInput::setAttribute(Attributes &attr, const wstring &key,
  const wstring &value)
{
	if (strCaseCmp(key, L"LABEL"))
		attr.label = value;
}

void DotGraphInput::mergeAttributes(Attributes &dst, const Attributes &src)
{
	if (!src.label.empty())
		dst.label = src.label;
}

void DotGraphInput::setVertexAttributes(Vertex *vertex, const Attributes &attr)
{
	if (!attr.label.empty())
		vertex->setText(attr.label);
}

void DotGraphInput::setEdgeAttributes(Edge *edge, const Attributes &attr)
{
	if (!attr.label.empty())
		edge->setText(attr.label);
}

Vertex* DotGraphInput::addVertex(const wstring &vertex)
{
	Vertex *v;
	map<std::wstring, Vertex*>::const_iterator it;

	it = _vertexes.find(vertex);
	if (it != _vertexes.end())
		return it->second;

	v = _graph->addVertex();
	v->setText(vertex);

	_vertexes.insert(pair<wstring, Vertex*>(vertex, v));

	return v;
}

Edge* DotGraphInput::addEdge(const wstring &src, const wstring &dst)
{
	return _graph->addEdge(_vertexes[src], _vertexes[dst]);
}


IO::Error DotGraphInput::readGraph(Graph *graph, const char *fileName,
  Encoding *encoding)
{
	IO::Error err = Ok;

	if (encoding) {
		locale lc(std::locale(), encoding->cvt());
		_fs.imbue(lc);
	}

	_fs.open(fileName);
	if (!_fs) {
		ioerr << "Error opening file: " << strerror(errno) << endl;
		return OpenError;
	}

	_graph = graph;

	if (!parse())
		err = FormatError;

	_vertexes.clear();
	attributesClear();
	_fs.close();
	if (_fs.bad())
		err = ReadError;

	if (err)
		_graph->clear();

	return err;
}
