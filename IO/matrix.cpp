#include <iostream>
#include <fstream>
#include <sstream>
#include "matrix.h"

using namespace std;


/*!
	\class MatrixGraphInput
	\brief Input provider for adjacency matrix graph representation.

	Lorem ipsum...
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
