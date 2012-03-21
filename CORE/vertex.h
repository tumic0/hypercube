#ifndef VERTEX_H_
#define VERTEX_H_

#include <string>
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

	Coordinates coordinates() const {return _coordinates;}
	void setCoordinates(const Coordinates &c);
	Margin margin() const {return _margin;}

	std::wstring text() const {return _text;}
	void setText(const std::wstring &text);
	Color color() const {return _color;}
	void setColor(const Color &color) {_color = color;}
	int size() const {return _size;}
	void setSize(int size);
	int fontSize() const {return _fontSize;}
	void setFontSize(int size);

private:
	void computeMargin();

	Coordinates _coordinates;
	Margin _margin;
	std::wstring _text;
	Color _color;
	int _size;
	int _fontSize;

	size_t _id;
	Graph *_graph;
};

#endif /* VERTEX_H_ */
