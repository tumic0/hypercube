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

	if (!_fs.good()) {
		_token = EOI;
		return;
	}

	_id.clear();

	while (1) {
		c = _fs.get();

		if (!_fs.good()) {
			if (state == 0)
				_token = EOI;
			else if (state == 2)
				_token = ID;
			else
				_token = ERROR;
			return;
		}

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
	_token = START;

	nextToken();

	while (1) {
		entry();

		switch (_token) {
			case NL:
				nextToken();
				break;
			case EOI:
				_vertexes.clear();
				return true;
			default:
				error();
				_vertexes.clear();
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
