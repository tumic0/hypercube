#include <sstream>
#include <cctype>
#include "utf8cvt.h"
#include "list.h"

using namespace std;


/*!
	\class ListGraphInput
	\brief Input provider for edge list graph representation.

	Loads graphs represented as edge lists. Each line of the input file
	represents an edge in the format

	\verbatim START_VERTEX END_VERTEX EDGE_VALUE \endverbatim
*/

void ListGraphInput::addVertex(wstring vertex)
{
	if (_vertexes.find(vertex) != _vertexes.end())
		return;

	int index = _graph->addVertex();
	wstringstream ss;
	ss << vertex;
	_graph->setVertexText(index, ss.str());

	_vertexes.insert(pair<wstring,int>(vertex, index));
}

void ListGraphInput::addEdge(wstring src, wstring dst, wstring val)
{
	wstringstream ss;
	int v1 = max(_vertexes[src], _vertexes[dst]);
	int v2 = min(_vertexes[src], _vertexes[dst]);

	ss << val;
	_graph->addEdge(v1, v2);
	_graph->setEdgeText(v1, v2, ss.str());
}

IO::Error ListGraphInput::readGraph(Graph *graph, const char *fileName)
{
	wifstream fs;
	locale utf8(std::locale(), new utf8cvt);
	wstring line, src, dst, value;
	int err = 0;

	fs.imbue(utf8);
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
