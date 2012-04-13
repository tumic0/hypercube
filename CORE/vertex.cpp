#include <cmath>
#include "vertex.h"
#include "misc.h"
#include "config.h"
#include "graph.h"

using namespace std;


Vertex::Vertex(Graph *graph, size_t id)
{
	_graph = graph;
	_id = id;

	_size = 0;
	_fontSize = 0;
}

void Vertex::setText(const std::wstring &text)
{
	_text = text;
	computeMargin();
}

void Vertex::setSize(int size)
{
	_size = size;
	computeMargin();
}

void Vertex::setFontSize(int size)
{
	_fontSize = size;
	computeMargin();
}

void Vertex::computeMargin()
{
	_margin.setRb(
	  Coordinates(
		_size + ceil(AVG_CHAR_WIDTH * _fontSize) * _text.length(),
		MAX(_size, _size / 2 + _fontSize))
	  );
	_graph->updateMargins(_id);
}

void Vertex::setCoordinates(const Coordinates &c)
{
	_coordinates = c;
	_graph->updateCoordinates(_id);
}
