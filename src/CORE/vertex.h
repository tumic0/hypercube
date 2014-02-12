#ifndef VERTEX_H_
#define VERTEX_H_

#include <string>
#include <map>
#include "coordinates.h"
#include "margin.h"
#include "color.h"

class Graph;

class Vertex
{
public:
	Vertex(Graph *graph, size_t id);

	size_t id() const {return _id;}
	Graph *graph() const {return _graph;}

	const Coordinates &coordinates() const {return _coordinates;}
	void setCoordinates(const Coordinates &c);
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

private:
	void computeMargin();

	Coordinates _coordinates;
	Margin _margin;
	Color _color;
	int _size;
	int _fontSize;

	size_t _id;
	Graph *_graph;

	std::wstring _text;
	std::wstring _attribute;
	std::map<std::wstring, std::wstring> _attributes;
};

#endif /* VERTEX_H_ */
