#ifndef IO_H_
#define IO_H_

#include "CORE/graph.h"


class IO
{
public:
	typedef enum {
		Ok,
		OpenError,
		ReadError,
		WriteError,
		FormatError,
	} Error;
};


class InputProvider : public IO
{
public:
	virtual Error readGraph(Graph *graph, const char *fileName) = 0;
};

class OutputProvider : public IO
{
public:
	virtual Error writeGraph(Graph *graph, const char *fileName) = 0;
	virtual const char* type() = 0;
	virtual const char* description() = 0;
};


extern OutputProvider* outputProviders[];
extern InputProvider* inputProviders[];

#endif /* IO_H_ */
