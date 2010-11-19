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
	void addVertex(int vertex);
	void addEdge(int src, int dst, int val);

	std::map<int,int> _vertexes;
	Graph *_graph;
};

#endif /* LIST_H_ */
