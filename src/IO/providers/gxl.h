#ifndef GXL_H_
#define GXL_H_

#include "IO/io.h"

class GxlGraphInput : public InputProvider
{
public:
	virtual Error readGraph(Graph *graph, const char *fileName);
};

#endif // GXL_H_
