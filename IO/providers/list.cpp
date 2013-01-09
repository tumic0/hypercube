#include <cstring>
#include <cerrno>
#include <cctype>
#include <fstream>
#include <sstream>
#include "CORE/vertex.h"
#include "CORE/edge.h"
#include "list.h"

using namespace std;


void ListGraphInput::error()
{
	if (_token == ERROR)
		return;

	ioerr << "LIST: parse error on line: " << _line << endl;
	_token = ERROR;
}

void ListGraphInput::nextToken()
{
	int c, state = 0;

	_id.clear();

	while (1) {
		c = _fs.get();

		switch (state) {
			case 0:
				if (c == ' ' || c == '\t')
					break;
				if (c == '\r') {
					state = 1;
					break;
				}
				if (c == '\n') {
					_line++;
					_token = NL;
					return;
				}
				if (c == '"') {
					state = 3;
					break;
				}
				if (c > 32) {
					_id += c;
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
				if (c == '\n') {
					_line++;
					_token = NL;
					return;
				}
				error();
				return;

			case 2:
				if (c > 32) {
					_id += c;
					break;
				}
				_fs.unget();
				_token = ID;
				return;

			case 3:
				if (c == '"') {
					_token = ID;
					return;
				}
				if (c == '\\') {
					state = 4;
					break;
				}
				_id += c;
				break;

			case 4:
				if (c == '"')
					_id += '"';
				else {
					_id += '\\';
					_id += c;
				}
				state = 3;
				break;
		}
	}
}

void ListGraphInput::compare(Token token)
{
	if (_token == token)
		nextToken();
	else
		error();
}

void ListGraphInput::edgeValue(Edge *edge)
{
	switch (_token) {
		case ID:
			edge->setText(_id);
			nextToken();
			break;
		case NL:
		case EOI:
			break;
		default:
			error();
	}
}

void ListGraphInput::entry()
{
	Vertex *src, *dst;
	Edge *edge;

	switch (_token) {
		case EOI:
			return;
		case ID:
			src = addVertex(_id);
			nextToken();
			dst = addVertex(_id);
			compare(ID);
			edge = _graph->addEdge(src, dst);
			edgeValue(edge);
			break;
		default:
			error();
	}
}

void ListGraphInput::list()
{
	while (1) {
		entry();

		switch (_token) {
			case NL:
				nextToken();
				break;
			case EOI:
				return;
			default:
				error();
				return;
		}
	}
}

bool ListGraphInput::parse()
{
	_line = 1;
	_token = START;

	nextToken();
	list();

	_vertexes.clear();

	return (_token == EOI) ? true : false;
}

Vertex* ListGraphInput::addVertex(const wstring &vertex)
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


IO::Error ListGraphInput::readGraph(Graph *graph, const char *fileName,
  Encoding *encoding)
{
	IO::Error err = Ok;

	_graph = graph;

	if (encoding) {
		locale lc(std::locale(), encoding->cvt());
		_fs.imbue(lc);
	}

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
		graph->clear();

	return err;
}
