#include <sstream>
#include <cctype>
#include "CORE/vertex.h"
#include "CORE/edge.h"
#include "list.h"


using namespace std;

void ListGraphInput::addVertex(wstring vertex)
{
	if (_vertexes.find(vertex) != _vertexes.end())
		return;

	Vertex *v = _graph->addVertex();
	wstringstream ss;
	ss << vertex;
	v->setText(ss.str());

	_vertexes.insert(pair<wstring,size_t>(vertex, v->id()));
}

void ListGraphInput::addEdge(wstring src, wstring dst, wstring val)
{
	wstringstream ss;
	size_t v1 = max(_vertexes[src], _vertexes[dst]);
	size_t v2 = min(_vertexes[src], _vertexes[dst]);

	ss << val;
	Edge *e = _graph->addEdge(_graph->vertex(v1), _graph->vertex(v2));
	e->setText(ss.str());
}

IO::Error ListGraphInput::readGraph(Graph *graph, const char *fileName,
  Encoding *encoding)
{
	wifstream fs;
	wstring line, src, dst, value;
	int err = 0;

	if (encoding) {
		locale lc(std::locale(), encoding->cvt());
		fs.imbue(lc);
	}

	fs.open(fileName);
	if (!fs)
		return OpenError;

	_graph = graph;

	while (getline(fs, line)) {
		wistringstream iss(line);
		iss >> src >> dst >> value;
		if (!iss) {
			err = 1;
			break;
		}

		addVertex(src);
		addVertex(dst);
		addEdge(src, dst, value);
	}

	fs.close();
	_vertexes.clear();

	if (err)
		return FormatError;
	else
		return Ok;
}
