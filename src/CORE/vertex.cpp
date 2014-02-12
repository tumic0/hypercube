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

void Vertex::setAttribute(const std::wstring &attribute)
{
	map<wstring, wstring>::const_iterator it;

	it = _attributes.find(attribute);
	if (it == _attributes.end()) {
		_attribute.clear();
		_text.clear();
	} else {
		_attribute = it->first;
		_text = it->second;
	}

	computeMargin();
}

void Vertex::addAttribute(const pair<wstring, wstring> &attribute)
{
	_attributes.insert(attribute);
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
	if (_fontSize)
		_margin.setRb(Coordinates(
			_size + (int)ceil(AVG_CHAR_WIDTH * _fontSize * text().length()),
			MAX(_size, _size / 2 + _fontSize)
		));
	else
		_margin.setRb(Coordinates(_size, _size));

	_graph->updateMargins(_id);
}

void Vertex::setCoordinates(const Coordinates &c)
{
	_coordinates = c;
	_graph->updateCoordinates(_id);
}
