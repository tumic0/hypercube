#ifndef PS_H_
#define PS_H_

#include <string>
#include <ostream>
#include "IO/io.h"

class PsSnippet;

class PsGraphOutput: public OutputProvider
{
public:
	virtual Error writeGraph(Graph *graph, const char *filename);
	virtual const char* type() {return "eps";}
	virtual const char* description() {return "PostScript";}

private:
	void prolog(Graph *graph, PsSnippet *sn, std::wofstream &fs);
	void edges(Graph *graph, std::wofstream &fs);
	void vertexes(Graph *graph, std::wofstream &fs);
	void legend(Graph *graph, std::wofstream &fs);
};

#endif /* PS_H_ */
