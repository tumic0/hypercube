#ifndef GXL_H
#define GXL_H

#include "IO/io.h"

class GxlGraphInput : public InputProvider
{
public:
	virtual Error readGraph(Graph *graph, const char *fileName,
	  Encoding *encoding);
};

#endif // GXL_H
