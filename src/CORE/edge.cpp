#include <cmath>
#include "config.h"
#include "graph.h"
#include "vertex.h"
#include "edge.h"

using namespace std;


Edge::Edge(Vertex *src, Vertex *dst, size_t id)
{
	_src = src;
	_dst = dst;
	_id = id;

	_size = 0;
	_fontSize = 0;
	_zValue = -1;
	_twin = false;
}

void Edge::setAttribute(const wstring &attribute)
{
	map<wstring, wstring>::const_iterator it;

	it = _attributes.find(attribute);
	if (it == _attributes.end()) {
		_text.clear();
		_attribute.clear();
	} else {
		_text = it->second;
		_attribute = it->first;
	}

	computeMargin();
}

void Edge::addAttribute(const pair<wstring, wstring> &attribute)
{
	_attributes.insert(attribute);
}

void Edge::setSize(int size)
{
	_size = size;
	computeMargin();
}

void Edge::setFontSize(int size)
{
	_fontSize = size;
	computeMargin();
}

void Edge::computeMargin()
{
	if (_fontSize) {
		Coordinates tm(
		  _size + (int)ceil(AVG_CHAR_WIDTH * _fontSize * text().length()),
		  _size / 2 + _fontSize
		);
		_margin.setRb(tm);
		_margin.setLt(tm);
	} else
		_margin = Margin();

	_src->graph()->updateMargins(_src->id());
	_dst->graph()->updateMargins(_dst->id());
}
