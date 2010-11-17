#ifndef LIST_H_
#define LIST_H_

#include <fstream>
#include <map>
#include "io.h"

class ListGraphInput: public InputProvider
{
public:
	virtual Error readGraph(Graph *graph, const char *fileName);

private:
	enum Token {
		LEX_INT, LEX_NL, LEX_EOI, LEX_ERR
	};

	void nextToken();
	void error();
	void compare(Token token);
	void addVertex(int vertex);
	void addEdge(int src, int dst, int val);
	void edge();

	Token _token;
	int _val;
	std::wifstream _fs;
	std::map<int,int> _vertexes;
	Graph *_graph;
};

#endif /* LIST_H_ */
