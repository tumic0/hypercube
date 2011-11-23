#include <cstdlib>
#include "graph.h"
#include "misc.h"

using namespace std;

#define INIT_SIZE		20
#define EXP_FACTOR		2
#define LINEAR_FACTOR	100
#define EXP_THRESHOLD	LINEAR_FACTOR

#define HUE_INIT		0.1f
#define HUE_INCREMENT	0.62f
#define SATURATION		0.99f
#define VALUE			0.99f


static inline bool lineSegmentCrossing(Coordinates l1p1, Coordinates l1p2,
  Coordinates l2p1, Coordinates l2p2);
static inline int euclideanDistanceSqr(Coordinates p1, Coordinates p2);
static unsigned hsv2rgb(float h, float s, float v);


/*!
	\class Graph
	\brief A class representing graphs.

	Lorem ipsum...
*/

Graph::Graph()
{
	_size = 0;
	_allocSize = 0;

	_dimensions = Coordinates(0, 0);

	_vertexes = 0;
	_matrix = 0;

	_hueState = HUE_INIT;
}

Graph::~Graph()
{
	delete[] _vertexes;

	for (int i = 0; i < _allocSize; i++) {
		for (int j = 0; j < i; j++)
			if (edge(i, j))
				delete _matrix[i][j].edge;
		delete[] _matrix[i];
	}
	delete[] _matrix;
}

void Graph::clear()
{
	for (int i = 0; i < _size; i++) {
		for (int j = 0; j < i; j++) {
			if (edge(i, j)) {
				delete _matrix[i][j].edge;
				_matrix[i][j].edge = 0;
				_matrix[i][j].distance = 0;
			}
		}
	}

	_size = 0;
	_dimensions = Coordinates(0, 0);
	_colors.clear();
	_hueState = HUE_INIT;
}

int Graph::addVertex()
{
	int v;

	if (_size == _allocSize)
		allocateMemory();
	updateDistance(_size);

	v = _size;
	_size++;

	return v;
}

void Graph::addEdge(int src, int dst)
{
	_matrix[src][dst].edge = new Edge();

	updateCrossings(src);
	updateCrossings(dst);
}

void Graph::center(void)
{
	Coordinates mn, mx, offset;

	mn = vertexCoordinates(0) - margin(0).lt();
	mx = vertexCoordinates(0) + margin(0).rb();

	for (int i = 1; i < _size; i++) {
		mn = min(mn, vertexCoordinates(i) - margin(i).lt());
		mx = max(mx, vertexCoordinates(i) + margin(i).rb());
	}

	offset = (_dimensions - mx - mn) / 2;

	for (int i = 0; i < _size; i++)
		_vertexes[i].setCoordinates(vertexCoordinates(i) + offset);
}

void Graph::randomize(void)
{
	for (int i = 0; i < _size; i++)
		moveVertex(i, Coordinates(
		  margin(i).lt().x() + rand()
		    % (dimensions().x() - margin(i).rb().x() - margin(i).lt().x()),
		  margin(i).lt().y() + rand()
		    % (dimensions().y() - margin(i).rb().y() - margin(i).lt().y())));
}

void Graph::colorize(void)
{
	Color color;
	map<wstring, Color>::iterator it;

	for (int i = 0; i < _size; i++) {
		for (int j = 0; j < i; j++) {
			if (edge(i, j)) {
				it = _colors.find(edgeText(i, j));

				if (it == _colors.end()) {
					color = Color(nextColor());
					_colors.insert(pair<wstring, Color>(edgeText(i, j), color));
				} else {
					color = (*it).second;
				}

				setEdgeColor(i, j, color);
			}
		}
	}
}

void Graph::bindTo(Graph *source)
{
	setDimensions(source->dimensions());

	for (int i = 0; i < _size; i++)
		for (int j = 0; j < source->size(); j++)
			if (source->vertexText(j) == _vertexes[i].text())
				moveVertex(i, source->vertexCoordinates(j));
}

void Graph::project(Graph *source)
{
	int found;

	for (int i = 0; i < _size; i++) {
		for (int j = 0; j < source->size(); j++) {
			if (source->vertexText(j) == _vertexes[i].text()) {
				setVertexColor(i, source->vertexColor(j));
				setVertexSize(i, source->vertexSize(j));
				setVertexFontSize(i, source->vertexFontSize(j));
			}
		}
	}

	for (int i = 0; i < _size; i++) {
		for (int j = 0; j < i; j++) {
			if (edge(i, j)) {
				found = 0;
				for (int k = 0; k < source->size(); k++) {
					for (int l = 0; l < k; l++) {
						if (source->edge(k, l)) {
							if ((source->vertexText(l) == _vertexes[j].text()
							  && source->vertexText(k) == _vertexes[i].text())
							  || (source->vertexText(l) == _vertexes[i].text()
							  && source->vertexText(k) == _vertexes[j].text())) {
								setEdgeColor(i, j, source->edgeColor(k, l));
								setEdgeSize(i, j, source->edgeSize(k, l));
								setEdgeFontSize(i, j, source->edgeFontSize(k, l));
								found = 1;
							}
						}
					}
				}
				if (!found)
					setEdgeZValue(i, j, -2);
			}
		}
	}
}

int Graph::crossings()
{
	int crossings = 0;

	for (int i = 0; i < _size; i++)
		for (int j = 0; j < i; j++)
			if (edge(i, j))
				crossings += _matrix[i][j].edge->crossings();

	return crossings;
}

float Graph::distance()
{
	float distance = 0;

	for (int i = 0; i < _size; i++)
		for (int j = 0; j < i; j++)
			distance += _matrix[i][j].distance;

	return distance;
}

float Graph::length()
{
	float length = 0;

	for (int i = 0; i < _size; i++)
		for (int j = 0; j < i; j++)
			if (edge(i, j))
				length += _matrix[i][j].edge->length();

	return  length;
}

void Graph::moveVertex(int v, const Coordinates &location)
{
	_vertexes[v].setCoordinates(location);

	updateCrossings(v);
	updateDistance(v);
}

void Graph::allocateMemory()
{
	int allocSize;
	Vertex *vertexes;
	MatrixItem **matrix;


	// Compute the new allocated size
	if (_allocSize == 0)
		allocSize = INIT_SIZE;
	else if (_allocSize < EXP_THRESHOLD)
		allocSize = _allocSize * EXP_FACTOR;
	else
		allocSize = _allocSize + LINEAR_FACTOR;

	// Allocate data structures for the graph representation
	vertexes = new Vertex[allocSize];
	matrix = new MatrixItem*[allocSize];
	for (int i = 0; i < allocSize; i++)
		matrix[i] = new MatrixItem[i];

	// Initialize the new items
	for (int i = _allocSize; i < allocSize; i++) {
		for (int j = 0; j < i; j++) {
			matrix[i][j].edge = 0;
			matrix[i][j].distance = 0;
		}
	}


	// If reallocating, copy the current values to the new structures
	if (allocSize > INIT_SIZE) {
		for (int i = 0; i < _allocSize; i++) {
			vertexes[i] = _vertexes[i];
			for (int j = 0; j < i; j++)
					matrix[i][j] = _matrix[i][j];
		}
	}

	// Free the old data structures if any
	if (allocSize > INIT_SIZE) {
		delete[] _vertexes;

		for (int i = 0; i < _allocSize; i++)
			delete[] _matrix[i];
		delete[] _matrix;
	}

	// Setup the object pointers to new structures
	_allocSize = allocSize;
	_vertexes = vertexes;
	_matrix = matrix;
}

void Graph::updateCrossings(int v1, int v2)
{
	_matrix[v1][v2].edge->setCrossings(0);

	for (int i = v1 + 1; i < _size; i++)
		for (int j = 0; j < i; j++)
			if (edge(i, j) && (v1 != j && v2 != j))
				if (lineSegmentCrossing(_vertexes[v1].coordinates(),
				  _vertexes[v2].coordinates(), _vertexes[i].coordinates(),
				  _vertexes[j].coordinates()))
					_matrix[v1][v2].edge->setCrossings(
					  _matrix[v1][v2].edge->crossings() + 1);
}

void Graph::updateCrossings(int v)
{
	for (int n = 0; n < v; n++)
		if (edge(v, n))
			updateCrossings(v, n);
	for (int n = v + 1; n < _size; n++)
		if (edge(n, v))
			updateCrossings(n, v);
}

void Graph::updateDistance(int v1, int v2)
{
	int distance = euclideanDistanceSqr(_vertexes[v1].coordinates(),
	  _vertexes[v2].coordinates());

	_matrix[v1][v2].distance = 1 / (float)distance;
	if (edge(v1, v2))
		_matrix[v1][v2].edge->setLength(distance);
}

void Graph::updateDistance(int v)
{
	for (int n = 0; n < v; n++)
		updateDistance(v, n);
	for (int n = v + 1; n < _size; n++)
		updateDistance(n, v);
}

void Graph::updateMargins(int v)
{
	Margin margin;

	for (int n = 0; n < v; n++)
		if (edge(v, n))
			margin = max(margin, _matrix[v][n].edge->margin());
	for (int n = v + 1; n < _size; n++)
		if (edge(n, v))
			margin = max(margin, _matrix[n][v].edge->margin());

	if (margin != Margin()) {
		margin.setRb(margin.rb() + Coordinates(0, _vertexes[v].size() / 2));
		margin.setLt(margin.lt() - Coordinates(0, _vertexes[v].size() / 2));
	}

	margin = max(margin, _vertexes[v].margin());

	_vertexes[v].setTotalMargin(margin);
}


void Graph::setVertexText(int v, const std::wstring &text)
{
	_vertexes[v].setText(text);
	updateMargins(v);
}

void Graph::setVertexSize(int v, int size)
{
	_vertexes[v].setSize(size);
	updateMargins(v);
}

void Graph::setVertexFontSize(int v, int size)
{
	_vertexes[v].setFontSize(size);
	updateMargins(v);
}


void Graph::setEdgeText(int v1, int v2, const std::wstring &text)
{
	_matrix[v1][v2].edge->setText(text);
	updateMargins(v1);
	updateMargins(v2);
}

void Graph::setEdgeSize(int v1, int v2, int size)
{
	_matrix[v1][v2].edge->setSize(size);
	updateMargins(v1);
	updateMargins(v2);
}

void Graph::setEdgeFontSize(int v1, int v2, int size)
{
	_matrix[v1][v2].edge->setFontSize(size);
	updateMargins(v1);
	updateMargins(v2);
}


void Graph::setVertexColor(const Color &color)
{
	for (int i = 0; i < _size; i++)
		setVertexColor(i, color);
}

void Graph::setVertexSize(int size)
{
	for (int i = 0; i < _size; i++)
		setVertexSize(i, size);
}

void Graph::setEdgeColor(const Color &color)
{
	for (int i = 0; i < _size; i++)
		for (int j = 0; j < i; j++)
			if (edge(i, j))
				setEdgeColor(i, j, color);
}

void Graph::setEdgeSize(int size)
{
	for (int i = 0; i < _size; i++)
		for (int j = 0; j < i; j++)
			if (edge(i, j))
				setEdgeSize(i, j, size);
}

void Graph::setVertexFontSize(int size)
{
	for (int i = 0; i < _size; i++)
		setVertexFontSize(i, size);
}

void Graph::setEdgeFontSize(int size)
{
	for (int i = 0; i < _size; i++)
		for (int j = 0; j < i; j++)
			if (edge(i, j))
				setEdgeFontSize(i, j, size);
}

Color Graph::nextColor()
{
	_hueState += HUE_INCREMENT;
	_hueState -= (int) _hueState;
	return Color(hsv2rgb(_hueState, SATURATION, VALUE));
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

unsigned hsv2rgb(float h, float s, float v)
{
	unsigned hi;
	float r = 0, g = 0, b = 0, p, q, t, f;

	hi = (unsigned)(h * 6.0f);
	f = h * 6.0f - hi;
	p = v * (1.0f - s);
	q = v * (1.0f - f * s);
	t = v * (1.0f - (1.0f - f) * s);

	switch (hi) {
		case 0:
			r = v; g = t; b = p;
			break;
		case 1:
			r = q; g = v; b = p;
			break;
		case 2:
			r = p; g = v; b = t;
			break;
		case 3:
			r = p; g = q; b = v;
			break;
		case 4:
			r = t; g = p; b = v;
			break;
		case 5:
			r = v; g = p; b = q;
			break;
	}

	return ((unsigned)(r * 256) << 16)
	  + ((unsigned)(g * 256) << 8)
	  + (unsigned)(b * 256);
}
