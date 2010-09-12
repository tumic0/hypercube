#ifndef SVG_H_
#define SVG_H_

#include "io.h"

class SvgGraphOutput: public OutputProvider
{
public:
	virtual Error writeGraph(Graph *graph, const char *filename);
	virtual const char* type() {return "svg";}
	virtual const char* description() {return "SVG";}
};

#endif /* SVG_H_ */
