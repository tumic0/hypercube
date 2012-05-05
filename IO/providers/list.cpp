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

		if (!_fs.good())
			c = -1;

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
				_fs.putback(c);
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
				if (c == -1) {
					error();
					return;
				}
				_id += c;
				break;

			case 4:
				if (c == -1) {
					error();
					return;
				}
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
			edge->setText(_id);
			compare(ID);
			break;
		default:
			error();
	}
}

bool ListGraphInput::parse()
{
	_line = 1;

	nextToken();

	while (1) {
		entry();

		switch (_token) {
			case NL:
				nextToken();
				break;
			case EOI:
				return true;
			default:
				error();
				return false;
		}
	}
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

	if (!parse())
		err = FormatError;

	_vertexes.clear();
	_fs.close();
	if (_fs.bad())
		err = ReadError;

	if (err)
		graph->clear();

	return err;
}
