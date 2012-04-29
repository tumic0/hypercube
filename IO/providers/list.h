#ifndef LIST_H_
#define LIST_H_

#include <map>
#include <string>
#include <fstream>
#include "IO/io.h"


class Vertex;


class ListGraphInput: public InputProvider
{
public:
	virtual Error readGraph(Graph *graph, const char *fileName,
	  Encoding *encoding);

private:
	enum Token {
		START,		/* Initial value */
		EOI,		/* End of File */
		ERROR,		/* Parse error */
		NL,			/* New line */
		ID			/* ID */
	};

	void error();
	void nextToken();
	void compare(Token token);
	void entry();
	bool parse();

	Vertex *addVertex(const std::wstring &vertex);

	std::wifstream _fs;
	std::wstring _id;
	Token _token;
	unsigned _line;

	std::map<std::wstring, Vertex*> _vertexes;
	Graph *_graph;
};

#endif /* LIST_H_ */
