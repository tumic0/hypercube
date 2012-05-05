#include <fstream>
#include <sstream>
#include "CORE/vertex.h"
#include "CORE/edge.h"
#include "matrix.h"


using namespace std;

IO::Error MatrixGraphInput::readGraph(Graph *graph, const char *fileName,
  Encoding*)
{
	size_t size, value;


	ifstream fs(fileName);
	if (!fs) {
		perror(fileName);
		return OpenError;
	}

	if (!(fs >> size)) {
		cerr << "MATRIX: parse error on line: 1" << endl;
		return FormatError;
	}


	for (size_t i = 0; i < size; i++) {
		wstringstream ss;
		ss << i;

		Vertex *v = graph->addVertex();
		v->setText(ss.str());
	}

	for (size_t i = 0; i < size; i++) {
		for (size_t j = 0; j < size; j++) {
			if (!(fs >> value))
				return FormatError;
			else
				if (value > 0) {
					wstringstream ss;
					ss << value;

					Edge *e = graph->addEdge(graph->vertex(i), graph->vertex(j));
					e->setText(ss.str());
				}
		}
	}

	fs.close();

	return Ok;
}
