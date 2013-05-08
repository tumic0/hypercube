#ifndef IO_H_
#define IO_H_

#include <string>
#include <sstream>
#include <fstream>
#include "CORE/graph.h"
#include "CORE/line.h"
#include "encoding.h"


std::wstring trim(const std::wstring &str);
bool stringCaseCmp(const std::wstring &s1, const std::wstring &s2);
void stringReplace(std::wstring &source, const std::wstring &find,
  const std::wstring &replace);
std::wstring s2w(const std::string &s);


class IO
{
public:
	enum Error {Ok, OpenError, ReadError, WriteError, FormatError};
	static std::ostringstream ioerr;
};


class InputProvider : public IO
{
public:
	virtual Error readGraph(Graph *graph, const char *fileName) = 0;
	virtual void setInputEncoding(Encoding *encoding);
	virtual void setNodeLabelAttribute(const char *name);
	virtual void setEdgeLabelAttribute(const char *name);
};

class OutputProvider : public IO
{
public:
	virtual Error writeGraph(Graph *graph, const char *fileName) = 0;
	virtual const char* type() = 0;
	virtual const char* description() = 0;

protected:
	struct Arrow {
		CoordinatesF p[3];
	};

	static CoordinatesF edgeTextPosition(const LineF &line, float size,
	  const CoordinatesF &textBox);
	static CoordinatesF vertexTextPosition(const CoordinatesF &point,
	  float size);
	static Arrow arrow(LineF &line, float size);
};

class lexstream : public std::wifstream
{
public:
	lexstream() : _unget(false) {}
	int get() {
		if (_unget)
			_unget = false;
		else
			_last = std::wifstream::get();
		return std::wifstream::good() ? _last : -1;
	}
	void unget() {_unget = true;}

private:
	int _last;
	bool _unget;
};

#endif /* IO_H_ */
