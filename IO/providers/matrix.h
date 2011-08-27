#ifndef MATRIX_H_
#define MATRIX_H_

#include "IO/io.h"

class MatrixGraphInput: public InputProvider
{
public:
	virtual Error readGraph(Graph *graph, const char *fileName,
	  Encoding *encoding);
};

#endif /* MATRIX_H_ */
