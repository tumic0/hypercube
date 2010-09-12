#ifndef MATRIX_H_
#define MATRIX_H_

#include "io.h"

class MatrixGraphInput: public InputProvider
{
public:
	virtual Error readGraph(Graph *graph, const char *fileName);
};

#endif /* MATRIX_H_ */
