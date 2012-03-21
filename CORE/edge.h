#ifndef EDGE_H_
#define EDGE_H_

#include <string>
#include "coordinates.h"
#include "margin.h"
#include "color.h"

class Vertex;

class Edge
{
public:
	Edge(Vertex *src, Vertex *dst, size_t id);

	size_t id() const {return _id;}
	Vertex *src() const {return _src;}
	Vertex *dst() const {return _dst;}

	Margin margin() const {return _margin;}

	std::wstring text() const {return _text;}
	void setText(const std::wstring &text);
	Color color() const {return _color;}
	void setColor(const Color &color) {_color = color;}
	int size() const {return _size;}
	void setSize(int size);
	int fontSize() const {return _fontSize;}
	void setFontSize(int size);
	int zValue() const {return _zValue;}
	void setZValue(int value) {_zValue = value;}
	bool directed() const {return _directed;}
	void setDirected(bool state) {_directed = state;}

private:
	void computeMargin();

	size_t _id;
	Vertex *_src;
	Vertex *_dst;

	Margin _margin;
	std::wstring _text;
	Color _color;
	int _size;
	int _fontSize;
	int _zValue;
	bool _directed;
};

#endif /* EDGE_H_ */
