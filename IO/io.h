#ifndef IO_H_
#define IO_H_

#include <string>
#include "CORE/graph.h"
#include "CORE/line.h"
#include "encoding.h"


class IO
{
public:
	enum Error {Ok, OpenError, ReadError, WriteError, FormatError};
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

	struct Arrow {
		CoordinatesF p[3];
	};

	static CoordinatesF edgeTextPosition(const LineF &line, float size,
	  const CoordinatesF &textBox);
	static CoordinatesF vertexTextPosition(const CoordinatesF &point,
	  float size);
	static Arrow arrow(LineF &line, float size);

	static void stringReplace(std::wstring &source, const std::wstring &find,
	  const std::wstring &replace);
};

#endif /* IO_H_ */
