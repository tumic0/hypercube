#ifndef LIST_H_
#define LIST_H_

#include <map>
#include <string>
#include "IO/io.h"

class ListGraphInput: public InputProvider
{
public:
	virtual Error readGraph(Graph *graph, const char *fileName,
	  Encoding *encoding);

private:
	void addVertex(const std::wstring &vertex);
	void addEdge(const std::wstring &src, const std::wstring &dst,
	  const std::wstring &val);

	unsigned _line;
	std::map<std::wstring, Vertex*> _vertexes;
	Graph *_graph;
};

#endif /* LIST_H_ */
