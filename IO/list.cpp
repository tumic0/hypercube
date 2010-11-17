#include <sstream>
#include <cctype>
#include "list.h"

using namespace std;


/*!
	\class ListGraphInput
	\brief Input provider for edge list graph representation.

	Loads graphs represented as edge list. Each line of the input file
	represents an edge in the format

	\verbatim VERTEX1 VERTEX2 VALUE \endverbatim
*/

void ListGraphInput::nextToken()
{
	int c, state = 0;

	while (1) {
		c = _fs.get();

		switch (state) {
			case 0:
				if (c == ' ' || c == '\t')
					break;
				if (isdigit(c)) {
					_val = c - '0';
					state = 1;
					break;
				}
				if (c == '\n') {
					_token = LEX_NL;
					return;
				}
				if (c == '\r') {
					state = 2;
					break;
				}
				if (c == EOF) {
					_token = LEX_EOI;
					return;
				}
				_token = LEX_ERR;
				return;
			case 1:
				if (isdigit(c)) {
					_val = _val * 10 + c - '0';
					break;
				}
				_fs.unget();
				_token = LEX_INT;
				return;
			case 2:
				if (c == '\n') {
					_token = LEX_NL;
					return;
				}
				_token = LEX_ERR;
				return;
		}
	}
}

void ListGraphInput::error()
{
	_token = LEX_ERR;
}

void ListGraphInput::compare(Token token) {
	if (_token != token)
		error();
	else
		nextToken();
}

void ListGraphInput::addVertex(int vertex)
{
	if (_vertexes.find(vertex) != _vertexes.end())
		return;

	int index = _graph->addVertex();
	wstringstream ss;
	ss << vertex;
	_graph->setVertexText(index, ss.str());

	_vertexes.insert(pair<int,int>(vertex, index));
}

void ListGraphInput::addEdge(int src, int dst, int val)
{
	wstringstream ss;
	int v1 = max(_vertexes[src], _vertexes[dst]);
	int v2 = min(_vertexes[src], _vertexes[dst]);

	ss << val;
	_graph->addEdge(v1, v2);
	_graph->setEdgeText(v1, v2, ss.str());
}

void ListGraphInput::edge()
{
	int src, dst, val;

	if (_token == LEX_EOI)
		return;

	src = _val;
	compare(LEX_INT);
	dst = _val;
	compare(LEX_INT);
	val = _val;
	compare(LEX_INT);

	switch (_token) {
		case LEX_NL:
			nextToken();
			break;
		case LEX_EOI:
			break;
		default:
			error();
	}

	addVertex(src);
	addVertex(dst);
	addEdge(src, dst, val);
}

IO::Error ListGraphInput::readGraph(Graph *graph, const char *fileName)
{
	_graph = graph;
	_fs.open(fileName);
	if (!_fs)
		return OpenError;

	nextToken();
	do {
		edge();
	} while (_token != LEX_EOI && _token != LEX_ERR);

	_fs.close();
	_vertexes.clear();

	if (_token == LEX_ERR) {
		_graph->clear();
		return FormatError;
	} else
		return Ok;
}
