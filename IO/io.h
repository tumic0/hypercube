#ifndef IO_H_
#define IO_H_

#include "CORE/graph.h"
#include "CORE/line.h"
#include "encoding.h"

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
	virtual Error readGraph(Graph *graph, const char *fileName,
	  Encoding *encoding) = 0;
};

class OutputProvider : public IO
{
public:
	virtual Error writeGraph(Graph *graph, const char *fileName) = 0;
	virtual const char* type() = 0;
	virtual const char* description() = 0;

	typedef struct {
		CoordinatesF p[3];
	} Arrow;

	static CoordinatesF edgeTextPosition(const LineF &line, float size,
	  float fontSize);
	static CoordinatesF vertexTextPosition(const CoordinatesF &point,
	  float size);
	static Arrow arrow(LineF &line, float size);
};

#endif /* IO_H_ */
