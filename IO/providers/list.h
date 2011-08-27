#ifndef LIST_H_
#define LIST_H_

#include <fstream>
#include <map>
#include <string>
#include "IO/io.h"

class ListGraphInput: public InputProvider
{
public:
	virtual Error readGraph(Graph *graph, const char *fileName,
	  Encoding *encoding);

private:
	void addVertex(std::wstring vertex);
	void addEdge(std::wstring src, std::wstring dst, std::wstring val);

	std::map<std::wstring,int> _vertexes;
	Graph *_graph;
};

#endif /* LIST_H_ */
