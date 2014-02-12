#ifndef IO_H_
#define IO_H_

#include <string>
#include <sstream>
#include <fstream>
#include "CORE/graph.h"
#include "CORE/line.h"
#include "encoding.h"


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
	virtual void setInputEncoding(Encoding *) {}
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
	static CoordinatesF legendRectSize(float size);
	static CoordinatesF legendPosition(int index, float size);
	static CoordinatesF legendTextPosition(const CoordinatesF &point,
	  float size);
};


class lexstream : public std::wifstream
{
public:
	lexstream() : _unget(false) {}
	void open(const char *filename, std::ios_base::openmode mode =
	  std::ios_base::in) {
		_unget = false;
		std::wifstream::open(filename, mode);
	}
	int get() {
		if (_unget) {
			if (is_open())
				_unget = false;
			else
				setstate(std::ios_base::badbit);
		} else
			_last = std::wifstream::get();
		return good() ? _last : -1;
	}
	void unget() {
		if (good() && is_open())
			_unget = true;
		else
			setstate(std::ios_base::badbit);
	}

private:
	int _last;
	bool _unget;
};

#endif /* IO_H_ */
