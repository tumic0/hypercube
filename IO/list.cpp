#include <sstream>
#include <cctype>
#include "list.h"

using namespace std;


/*!
	\class ListGraphInput
	\brief Input provider for edge list graph representation.

	Loads graphs represented as edge lists. Each line of the input file
	represents an edge in the format

	\verbatim START_VERTEX END_VERTEX EDGE_VALUE \endverbatim
*/

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

IO::Error ListGraphInput::readGraph(Graph *graph, const char *fileName)
{
	ifstream fs(fileName);
	if (!fs)
		return OpenError;

	_graph = graph;

	string line;
	int src, dst, value;
	int err = 0;
	while (getline(fs, line)) {
		istringstream iss(line);
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
