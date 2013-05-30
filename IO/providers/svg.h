#ifndef SVG_H_
#define SVG_H_

#include <string>
#include <ostream>
#include "IO/io.h"

class SvgGraphOutput: public OutputProvider
{
public:
	virtual Error writeGraph(Graph *graph, const char *filename);
	virtual const char* type() {return "svg";}
	virtual const char* description() {return "SVG";}

private:
	void escape(std::wstring &str);
	void header(Graph *graph, std::wofstream &fs);
	void footer(std::wofstream &fs);
	void edges(Graph *graph, std::wofstream &fs);
	void vertexes(Graph *graph, std::wofstream &fs);
	void legend(Graph *graph, std::wofstream &fs);
};

#endif /* SVG_H_ */
