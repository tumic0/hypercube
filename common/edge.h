#ifndef EDGE_H_
#define EDGE_H_

#include <string>
#include "coordinates.h"
#include "margin.h"
#include "color.h"


class Edge
{
public:
	Edge();

	int crossings() {return _crossings;}
	void setCrossings(int crossings) {_crossings = crossings;}
	float length() {return _length;}
	void setLength(float length) {_length = length;}

	Margin margin() const {return _margin;}

	std::wstring text() const {return _text;}
	void setText(const std::wstring &text);
	Color color() const {return _color;}
	void setColor(const Color &color) {_color = color;}
	int size() {return _size;}
	void setSize(int size);
	int fontSize() {return _fontSize;}
	void setFontSize(int size);
	int zValue() {return _zValue;}
	void setZValue(int value) {_zValue = value;}

private:
	void computeMargin();

	int _crossings;
	float _length;

	Margin _margin;

	std::wstring _text;
	Color _color;
	int _size;
	int _fontSize;
	int _zValue;
};

#endif /* EDGE_H_ */
