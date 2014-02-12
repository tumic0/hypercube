#ifndef GRAPHML_H_
#define GRAPHML_H_

#include "IO/io.h"

class GraphmlGraphInput : public InputProvider
{
public:
	virtual Error readGraph(Graph *graph, const char *fileName);

private:
	std::wstring _nodeLabelAttr, _edgeLabelAttr;
};

#endif // GRAPHML_H_
