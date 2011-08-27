#ifndef PS_H_
#define PS_H_

#include "IO/io.h"

class PsGraphOutput: public OutputProvider
{
public:
	virtual Error writeGraph(Graph *graph, const char *filename);
	virtual const char* type() {return "eps";}
	virtual const char* description() {return "PostScript";}
};

#endif /* PS_H_ */
