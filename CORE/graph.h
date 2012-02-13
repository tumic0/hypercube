#ifndef GRAPH_H_
#define GRAPH_H_

#include <vector>
#include <string>
#include "adjlist.h"
#include "container.h"
#include "coordinates.h"
#include "color.h"
#include "colormap.h"


class Vertex;
class Edge;

class Graph
{
public:
	Vertex *addVertex();
	Edge *addEdge(Vertex *src, Vertex *dst);

	size_t vertex_size() {return _vertexes.size();}
	size_t edge_size() {return _edges.size();}
	Vertex *vertex(size_t id) {return _vertexes[id];}
	Edge *edge(size_t id) {return _edges[id];}

	Coordinates dimensions(void) const {return _dimensions;}
	void setDimensions(const Coordinates &dimensions)
	  {_dimensions = dimensions;}

	void updateCoordinates(size_t vid);
	void updateMargins(size_t vid);

	Margin margin(size_t vid) const {return _margins[vid];}
	unsigned crossings() {return _crossings.sum();}
	float distance() {return _distances.sum();}
	float length() {return _lengths.sum();}

	void clear();
	void center();
	void randomize();
	void colorize();

	void bindTo(Graph *source);
	void project(Graph *source);

	void setVertexColor(const Color &color);
	void setVertexSize(int size);
	void setEdgeColor(const Color &color);
	void setEdgeSize(int size);
	void setVertexFontSize(int size);
	void setEdgeFontSize(int size);

private:

	void updateDistance(size_t vid);
	void updateCrossings(size_t eid);
	void updateLength(size_t eid);

	Coordinates _dimensions;

	std::vector<Vertex*> _vertexes;
	std::vector<Edge*> _edges;
	AdjacencyList _neighbours;

	LinearContainer<float> _lengths;
	MatrixContainer<float> _distances;
	MatrixContainer<unsigned> _crossings;
	std::vector<Margin> _margins;

	ColorMap _colormap;
};

#endif /* GRAPH_H_ */
