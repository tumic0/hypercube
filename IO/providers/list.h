#ifndef LIST_H_
#define LIST_H_

#include <map>
#include <string>
#include "IO/io.h"


class Vertex;
class Edge;


class ListGraphInput: public InputProvider
{
public:
	ListGraphInput() : _encoding(0) {}
	virtual Error readGraph(Graph *graph, const char *fileName);
	virtual void setInputEncoding(Encoding *encoding);

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
	void edgeValue(Edge *edge);
	void entry();
	void list();
	bool parse();

	Vertex *addVertex(const std::wstring &vertex);

	std::wstring _id;
	lexstream _fs;
	Token _token;
	unsigned _line;

	std::map<std::wstring, Vertex*> _vertexes;
	Graph *_graph;
	Encoding *_encoding;
};

#endif /* LIST_H_ */
