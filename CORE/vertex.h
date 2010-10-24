#ifndef VERTEX_H_
#define VERTEX_H_

#include <string>
#include "coordinates.h"
#include "margin.h"
#include "color.h"


class Vertex
{
public:
	Vertex();

	Coordinates coordinates() const {return _coordinates;}
	void setCoordinates(const Coordinates &c) {_coordinates = c;}
	Margin totalMargin() const {return _totalMargin;}
	void setTotalMargin(const Margin &m) {_totalMargin = m;}

	Margin margin() const {return _margin;}

	std::wstring text() const {return _text;}
	void setText(const std::wstring &text);
	Color color() const {return _color;}
	void setColor(const Color &color) {_color = color;}
	int size() {return _size;}
	void setSize(int size);
	int fontSize() {return _fontSize;}
	void setFontSize(int size);

private:
	void computeMargin();

	Coordinates _coordinates;
	Margin _margin;
	Margin _totalMargin;

	std::wstring _text;
	Color _color;
	int _size;
	int _fontSize;
};

#endif /* VERTEX_H_ */
