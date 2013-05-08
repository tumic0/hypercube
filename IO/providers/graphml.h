#ifndef GRAPHML_H_
#define GRAPHML_H_

#include "IO/io.h"

class GraphmlGraphInput : public InputProvider
{
public:
	virtual Error readGraph(Graph *graph, const char *fileName);
	virtual void setNodeLabelAttribute(const char *name);
	virtual void setEdgeLabelAttribute(const char *name);

private:
	std::wstring _nodeLabelAttr, _edgeLabelAttr;
};

#endif // GRAPHML_H_
