#include <cstring>
#include <cerrno>
#include <cctype>
#include <fstream>
#include <sstream>
#include "CORE/vertex.h"
#include "CORE/edge.h"
#include "list.h"

using namespace std;


void ListGraphInput::addVertex(const wstring &vertex)
{
	if (_vertexes.find(vertex) != _vertexes.end())
		return;

	Vertex *v = _graph->addVertex();
	v->setText(vertex);

	_vertexes.insert(pair<wstring, Vertex*>(vertex, v));
}

void ListGraphInput::addEdge(const wstring &src, const wstring &dst,
  const wstring &val)
{
	Edge *e = _graph->addEdge(_vertexes[src], _vertexes[dst]);
	e->setText(val);
}

IO::Error ListGraphInput::readGraph(Graph *graph, const char *fileName,
  Encoding *encoding)
{
	wifstream fs;
	wstring line, src, dst, value;
	IO::Error err = Ok;

	if (encoding) {
		locale lc(std::locale(), encoding->cvt());
		fs.imbue(lc);
	}

	fs.open(fileName);
	if (!fs) {
		ioerr << "Error opening file: " << strerror(errno) << endl;
		return OpenError;
	}

	_graph = graph;
	_line = 0;

	while (getline(fs, line)) {
		_line++;
		wistringstream iss(line);
		iss >> src >> dst >> value;
		if (!iss) {
			ioerr << "LIST: parse error on line: " << _line << endl;
			err = FormatError;
			break;
		}

		addVertex(src);
		addVertex(dst);
		addEdge(src, dst, value);
	}

	_vertexes.clear();
	fs.close();
	if (fs.bad())
		err = ReadError;

	if (err)
		graph->clear();

	return err;
}
