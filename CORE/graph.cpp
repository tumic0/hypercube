#include <cstdlib>
#include "vertex.h"
#include "edge.h"
#include "graph.h"
#include "misc.h"

using namespace std;


static inline bool lineSegmentCrossing(Coordinates l1p1, Coordinates l1p2,
  Coordinates l2p1, Coordinates l2p2);
static inline int euclideanDistanceSqr(Coordinates p1, Coordinates p2);


void Graph::clear()
{
	_dimensions = Coordinates(0, 0);

	_vertexes.clear();
	_edges.clear();
	_neighbours.clear();

	_distances.clear();
	_crossings.clear();
	_lengths.clear();

	_margins.clear();
	_colormap.clear();
}

Vertex* Graph::addVertex()
{
	Vertex *v = new Vertex(this, _vertexes.size());
	_vertexes.push_back(v);
	_neighbours.addVertex();

	_margins.push_back(Margin());
	_distances.increment();

	updateDistance(v->id());

	return v;
}

Edge* Graph::addEdge(Vertex *src, Vertex *dst)
{
	Edge *e = new Edge(src, dst, _edges.size());
	_edges.push_back(e);
	_neighbours.addEdge(e);

	_crossings.increment();
	_lengths.increment();

	updateCrossings(e->id());
	updateLength(e->id());

	return e;
}

void Graph::center(void)
{
	Coordinates mn, mx, offset;

	if (!vertex_size())
		return;

	mn = vertex(0)->coordinates() - margin(0).lt();
	mx = vertex(0)->coordinates() + margin(0).rb();

	for (size_t i = 1; i < _vertexes.size(); i++) {
		mn = min(mn, vertex(i)->coordinates() - margin(i).lt());
		mx = max(mx, vertex(i)->coordinates() + margin(i).rb());
	}

	offset = (dimensions() - mx - mn) / 2;

	for (size_t i = 0; i < _vertexes.size(); i++)
		vertex(i)->setCoordinates(vertex(i)->coordinates() + offset);
}

void Graph::randomize(void)
{
	for (size_t i = 0; i < vertex_size(); i++)
		vertex(i)->setCoordinates(Coordinates(
		  margin(i).lt().x() + rand()
		    % (dimensions().x() - margin(i).rb().x() - margin(i).lt().x()),
		  margin(i).lt().y() + rand()
			% (dimensions().y() - margin(i).rb().y() - margin(i).lt().y()))
		);
}

void Graph::colorize(void)
{
	for (size_t i = 0; i < edge_size(); i++)
		edge(i)->setColor(_colormap.color(edge(i)->text()));
}

void Graph::bindTo(Graph *source)
{
	for (size_t i = 0; i < vertex_size(); i++)
		for (size_t j = 0; j < source->vertex_size(); j++)
			if (source->vertex(j)->text() == _vertexes[i]->text())
				_vertexes[i]->setCoordinates(source->vertex(j)->coordinates());

}

void Graph::project(Graph *source)
{
	bool found;

	for (size_t i = 0; i < vertex_size(); i++) {
		for (size_t j = 0; j < source->vertex_size(); j++) {
			if (source->vertex(j)->text() == _vertexes[i]->text()) {
				vertex(i)->setColor(source->vertex(j)->color());
				vertex(i)->setSize(source->vertex(j)->size());
				vertex(i)->setFontSize(source->vertex(j)->fontSize());
			}
		}
	}

	for (size_t i = 0; i < edge_size(); i++) {
		found = false;
		for (size_t j = 0; j < source->edge_size(); j++) {
			if ((source->edge(j)->src()->text() == edge(i)->src()->text()
			  && source->edge(j)->dst()->text() == edge(i)->dst()->text())
			  || (source->edge(j)->src()->text() == edge(i)->dst()->text()
			  && source->edge(j)->dst()->text() == edge(i)->src()->text())) {
				edge(i)->setColor(source->edge(j)->color());
				edge(i)->setSize(source->edge(j)->size());
				edge(i)->setFontSize(source->edge(j)->fontSize());
				found = true;
			}
		}
		if (!found)
			edge(i)->setZValue(-2);
	}
}

void Graph::updateCoordinates(size_t vid)
{
	AdjacencyList::edge_iterator it;

	for (it = _neighbours.begin(vid); it != _neighbours.end(vid); it++) {
		updateCrossings(*it);
		updateLength(*it);
	}

	updateDistance(vid);
}

void Graph::updateMargins(size_t vid)
{
	Margin m;

	for (size_t i = 0; i < _edges.size(); i++)
		m = max(m, _edges[i]->margin());

	if (m != Margin()) {
		m.setRb(m.rb() + Coordinates(0, _vertexes[vid]->size() / 2));
		m.setLt(m.lt() - Coordinates(0, _vertexes[vid]->size() / 2));
	}

	m = max(m, _vertexes[vid]->margin());

	_margins[vid] = m;
}


void Graph::updateCrossings(size_t eid)
{
	for (size_t i = 0; i < _edges.size(); i++) {
		if (_edges[eid]->src()->id() == _edges[i]->src()->id()
		  || _edges[eid]->src()->id() == _edges[i]->dst()->id()
		  || _edges[eid]->dst()->id() == _edges[i]->src()->id()
		  || _edges[eid]->dst()->id() == _edges[i]->dst()->id())
			continue;

		if (lineSegmentCrossing(
		  _edges[eid]->src()->coordinates(),
		  _edges[eid]->dst()->coordinates(),
		  _edges[i]->src()->coordinates(),
		  _edges[i]->dst()->coordinates()))
			_crossings.setValue(eid, i, 1);
		else
			_crossings.setValue(eid, i, 0);
	}
}

void Graph::updateLength(size_t eid)
{
	int dist = euclideanDistanceSqr(_edges[eid]->src()->coordinates(),
	  _edges[eid]->dst()->coordinates());
	_lengths.setValue(eid, (float)dist);
}

void Graph::updateDistance(size_t vid)
{
	int dist;

	for (size_t i = 0; i < _vertexes.size(); i++) {
		if (i == vid)
			continue;

		dist = euclideanDistanceSqr(_vertexes[vid]->coordinates(),
		  _vertexes[i]->coordinates());
		if (dist != 0)
			_distances.setValue(vid, i, 1.0f / (float)dist);
	}
}


void Graph::setVertexColor(const Color &color)
{
	for (size_t i = 0; i < _vertexes.size(); i++)
		_vertexes[i]->setColor(color);
}

void Graph::setVertexSize(int size)
{
	for (size_t i = 0; i < _vertexes.size(); i++)
		_vertexes[i]->setSize(size);
}

void Graph::setEdgeColor(const Color &color)
{
	for (size_t i = 0; i < _edges.size(); i++)
		_edges[i]->setColor(color);
}

void Graph::setEdgeSize(int size)
{
	for (size_t i = 0; i < _edges.size(); i++)
		_edges[i]->setSize(size);
}

void Graph::setVertexFontSize(int size)
{
	for (size_t i = 0; i < _vertexes.size(); i++)
		_vertexes[i]->setFontSize(size);
}

void Graph::setEdgeFontSize(int size)
{
	for (size_t i = 0; i < _edges.size(); i++)
		_edges[i]->setFontSize(size);
}


bool lineSegmentCrossing(Coordinates l1p1, Coordinates l1p2,
  Coordinates l2p1, Coordinates l2p2)
{
	int ua_t, ub_t, u_b;

	ua_t = (l2p2.x() - l2p1.x()) * (l1p1.y() - l2p1.y())
	  - (l2p2.y() - l2p1.y()) * (l1p1.x() - l2p1.x());
	ub_t = (l1p2.x() - l1p1.x()) * (l1p1.y() - l2p1.y())
	  - (l1p2.y() - l1p1.y()) * (l1p1.x() - l2p1.x());
	u_b = (l2p2.y() - l2p1.y()) * (l1p2.x() - l1p1.x())
	  - (l2p2.x() - l2p1.x()) * (l1p2.y() - l1p1.y());

	if (u_b == 0) {
		if (ua_t == 0 && ub_t == 0) {
			int lx = MIN(l1p1.x(), l1p1.x() + (l1p2.x() - l1p1.x()));
			int ux = MAX(l1p1.x(), l1p1.x() + (l1p2.x() - l1p1.x()));
			int ly = MIN(l1p1.y(), l1p1.y() + (l1p2.y() - l1p1.y()));
			int uy = MAX(l1p1.y(), l1p1.y() + (l1p2.y() - l1p1.y()));

			if (((l2p1.x() >= lx && l2p1.x() <= ux)
			  || (l2p2.x() >= lx && l2p2.x() <= ux))
			  && ((l2p1.y() >= ly && l2p1.y() <= uy)
			  || (l2p2.y() >= ly && l2p2.y() <= uy)))
				return true;
		}
	} else {
		float ua = (float)ua_t / (float)u_b;
		float ub = (float)ub_t / (float)u_b;

		if (0 <= ua && ua <= 1 && 0 <= ub && ub <= 1)
			return true;
	}

	return false;
}

int euclideanDistanceSqr(Coordinates p1, Coordinates p2)
{
	return abs(((p1.x() - p2.x()) * (p1.x() - p2.x()))
	  + ((p1.y() - p2.y()) * (p1.y() - p2.y())));
}
