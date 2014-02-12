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

	const Margin &margin() const {return _margin;}

	const std::wstring &text() const {return _text;}
	const std::wstring &attribute() const {return _attribute;}
	void setAttribute(const std::wstring &attribute);
	void addAttribute(const std::pair<std::wstring, std::wstring> &attribute);
	const std::map<std::wstring, std::wstring> &attributes() const
	  {return _attributes;}

	const Color &color() const {return _color;}
	void setColor(const Color &color) {_color = color;}
	int size() const {return _size;}
	void setSize(int size);
	int fontSize() const {return _fontSize;}
	void setFontSize(int size);
	int zValue() const {return _zValue;}
	void setZValue(int value) {_zValue = value;}
	bool twin() const {return _twin;}
	void setTwin(bool state) {_twin = state;}

private:
	void computeMargin();

	size_t _id;
	Vertex *_src;
	Vertex *_dst;

	std::wstring _text;
	std::wstring _attribute;
	std::map<std::wstring, std::wstring> _attributes;

	Margin _margin;
	Color _color;
	int _size;
	int _fontSize;
	int _zValue;
	bool _twin;
};

#endif /* EDGE_H_ */
