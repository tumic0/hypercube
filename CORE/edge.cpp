#include "edge.h"
#include "config.h"


Edge::Edge() {
	_crossings = 0;
	_length = 0;

	_size = 0;
	_fontSize = 0;
	_zValue = -1;
}

void Edge::setText(const std::wstring &text)
{
	_text = text;
	computeMargin();
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
		_margin.setRb(
		  Coordinates(_size + (int)(AVG_CHAR_WIDTH * _fontSize)
		  * _text.length(), _size / 2 + _fontSize));
		_margin.setLt(
		  Coordinates(0, _size / 2 + _fontSize));
	} else
		_margin = Margin();
}
