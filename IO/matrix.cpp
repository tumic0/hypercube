#include <iostream>
#include <fstream>
#include <sstream>
#include "matrix.h"

using namespace std;

/*!
	\class MatrixGraphInput
	\brief Input provider for adjacency matrix graph representation.

	Loads graphs represented as adjacency matrixes. The adjacency matrix
	representation starts with a single number on a separate line representing
	the number of vertexes of the graph. Starting with the next line, the
	adjacency matrix itself follows. Numbers > 0 are taken as edges with the
	given edge value.

	Example:

	\verbatim
	3
	0 1 1
	1 0 1
	1 1 0
	\endverbatim
*/


IO::Error MatrixGraphInput::readGraph(Graph *graph, const char *fileName)
{
	int size, value;
	wstringstream ss;


	ifstream fs(fileName);
	if (!fs)
		return OpenError;

	if (!(fs >> size))
		return FormatError;


	for (int i = 0; i < size; i++) {
		ss.str(L"");
		ss << i;
		graph->setVertexText(graph->addVertex(), ss.str());
	}


	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (!(fs >> value))
				return FormatError;
			else
				if (value > 0 && j < i) {
					ss.str(L"");
					ss << value;
					graph->addEdge(i, j);
					graph->setEdgeText(i, j, ss.str());
				}
		}
	}

	fs.close();

	return Ok;
}
