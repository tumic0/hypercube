#ifndef GRAPH_H_
#define GRAPH_H_

#include <string>
#include "coordinates.h"
#include "color.h"
#include "vertex.h"
#include "edge.h"


class Graph
{
public:
	Graph();
	~Graph();

	int addVertex();
	void addEdge(int v1, int v2);
	bool edge(int v1, int v2)
	  {return (_matrix[v1][v2].edge == NULL) ? false : true;}

	int size(void) {return _size;}

	void moveVertex(int v, const Coordinates &location);
	Coordinates vertexCoordinates(int v) const
	  {return _vertexes[v].coordinates();}
	Margin margin(int v) const
	  {return _vertexes[v].totalMargin();}
	Coordinates dimensions(void) const {return _dimensions;}
	void setDimensions(const Coordinates &dimensions)
	  {_dimensions = dimensions;}

	int crossings(void);
	float distance(void);
	float length(void);

	void clear();
	void center(void);
	void randomize(void);
	void bindTo(Graph *source);
	void project(Graph *source);

	std::wstring vertexText(int v) const
	  {return _vertexes[v].text();}
	void setVertexText(int v, const std::wstring &text);
	Color vertexColor(int v) const
	  {return _vertexes[v].color();}
	void setVertexColor(int v, const Color &color)
	  {_vertexes[v].setColor(color);}
	int vertexSize(int v)
	  {return _vertexes[v].size();}
	void setVertexSize(int v, int size);
	int vertexFontSize(int v)
	  {return _vertexes[v].fontSize();}
	void setVertexFontSize(int v, int size);

	std::wstring edgeText(int v1, int v2) const
	  {return _matrix[v1][v2].edge->text();}
	void setEdgeText(int v1, int v2, const std::wstring &text);
	Color edgeColor(int v1, int v2) const
	  {return _matrix[v1][v2].edge->color();}
	void setEdgeColor(int v1, int v2, const Color &color)
	  {_matrix[v1][v2].edge->setColor(color);}
	int edgeSize(int v1, int v2)
	  {return _matrix[v1][v2].edge->size();}
	void setEdgeSize(int v1, int v2, int size);
	int edgeFontSize(int v1, int v2)
	  {return _matrix[v1][v2].edge->fontSize();}
	void setEdgeFontSize(int v1, int v2, int size);
	int edgeZValue(int v1, int v2)
	  {return _matrix[v1][v2].edge->zValue();}
	void setEdgeZValue(int v1, int v2, int value)
	  {_matrix[v1][v2].edge->setZValue(value);}

	void setVertexColor(const Color &color);
	void setVertexSize(int size);
	void setEdgeColor(const Color &color);
	void setEdgeSize(int size);
	void setVertexFontSize(int size);
	void setEdgeFontSize(int size);

private:

	struct MatrixItem
	{
		Edge *edge;
		float distance;
	};

	void allocateMemory();

	void updateCrossings(int v);
	void updateCrossings(int v1, int v2);
	void updateDistance(int v);
	void updateDistance(int v1, int v2);
	void updateMargins(int v);


	int _size;
	int _allocSize;

	Coordinates _dimensions;

	Vertex *_vertexes;
	MatrixItem **_matrix;
};

#endif /* GRAPH_H_ */
