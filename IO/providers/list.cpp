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

	ss << val;
	Edge *e = _graph->addEdge(_graph->vertex(_vertexes[src]),
	  _graph->vertex(_vertexes[dst]));
	e->setText(ss.str());
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
	if (!fs)
		return OpenError;

	_graph = graph;

	while (getline(fs, line)) {
		wistringstream iss(line);
		iss >> src >> dst >> value;
		if (!iss) {
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
