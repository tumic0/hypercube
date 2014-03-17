#include <cstdlib>
#include "vertex.h"
#include "edge.h"
#include "line.h"
#include "graph.h"
#include "misc.h"

using namespace std;


static const Coordinates max(const Coordinates &a, const Coordinates &b)
{
	return Coordinates(MAX(a.x(), b.x()), MAX(a.y(), b.y()));
}
static const Coordinates min(const Coordinates &a, const Coordinates &b)
{
	return Coordinates(MIN(a.x(), b.x()), MIN(a.y(), b.y()));
}
static const Margin max(const Margin &a, const Margin &b)
{
	return Margin(max(a.lt(), b.lt()), max(a.rb(), b.rb()));
}


Graph::Graph() : _legend(&_colormap)
{
	_dimensions = Coordinates(0, 0);
	_directed = false;
}

void Graph::clear()
{
	_dimensions = Coordinates(0, 0);
	_directed = false;

	_vertexes.clear();
	_edges.clear();
	_neighbours.clear();

	_distances.clear();
	_crossings.clear();
	_lengths.clear();

	_margins.clear();
	_colormap.clear();

	_legend.updateMap();
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

	checkTwin(e);
	_edges.push_back(e);
	_neighbours.addEdge(e);

	_crossings.increment();
	_lengths.increment();

	updateCrossings(e->id());
	updateLength(e->id());

	return e;
}

void Graph::checkTwin(Edge *e)
{
	for (size_t i = 0; i < _edges.size(); i++) {
		if (edge(i)->dst()->id() == e->src()->id()
		  && edge(i)->src()->id() == e->dst()->id()) {
			edge(i)->setTwin(true);
			e->setTwin(true);
			break;
		}
	}
}

void Graph::center(void)
{
	Coordinates mn, mx, tmp, offset, legend;

	if (!vertex_size())
		return;

	legend = _legend.dimensions();

	mn = vertex(0)->coordinates() - margin(0).lt();
	if (mn.y() <= legend.y())
		mn.setX(mn.x() - legend.x());
	mx = vertex(0)->coordinates() + margin(0).rb();

	for (size_t i = 1; i < _vertexes.size(); i++) {
		tmp = vertex(i)->coordinates() - margin(i).lt();
		if (tmp.y() <= legend.y())
			tmp.setX(tmp.x() - legend.x());
		mn = min(mn, tmp);
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
	_colormap.clear();
	for (size_t i = 0; i < edge_size(); i++)
		edge(i)->setColor(_colormap.color(edge(i)->text()));

	_legend.updateMap();
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

	m = max(m, _vertexes[vid]->margin());

	_margins[vid] = m;
}


void Graph::updateCrossings(size_t eid)
{
	Line l1(edge(eid)->src()->coordinates(), edge(eid)->dst()->coordinates());

	for (size_t i = 0; i < _edges.size(); i++) {
		if (_edges[eid]->src()->id() == _edges[i]->src()->id()
		  || _edges[eid]->src()->id() == _edges[i]->dst()->id()
		  || _edges[eid]->dst()->id() == _edges[i]->src()->id()
		  || _edges[eid]->dst()->id() == _edges[i]->dst()->id())
			continue;

		Line l2(edge(i)->src()->coordinates(), edge(i)->dst()->coordinates());

		if (l1.crossing(l2))
			_crossings.setValue(eid, i, 1);
		else
			_crossings.setValue(eid, i, 0);
	}
}

void Graph::updateLength(size_t eid)
{
	Line line(edge(eid)->src()->coordinates(), edge(eid)->dst()->coordinates());

	_lengths.setValue(eid, (float)line.lengthSqr());
}

void Graph::updateDistance(size_t vid)
{
	for (size_t i = 0; i < _vertexes.size(); i++) {
		if (i == vid)
			continue;

		Line line(_vertexes[vid]->coordinates(), _vertexes[i]->coordinates());
		int dist = line.lengthSqr();

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

void Graph::setVertexAttribute(const std::wstring &attribute)
{
	for (size_t i = 0; i < _vertexes.size(); i++)
		_vertexes[i]->setAttribute(attribute);
}

void Graph::setEdgeAttribute(const std::wstring &attribute)
{
	for (size_t i = 0; i < _edges.size(); i++)
		_edges[i]->setAttribute(attribute);
}
