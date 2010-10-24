#include "vertex.h"
#include "misc.h"
#include "config.h"


/*!
	\class Vertex
	\brief Defines vertex objects of a Graph.

	This class is used for Graph internal purposes only.
*/


Vertex::Vertex()
{
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
		Coordinates(_size + (int)(AVG_CHAR_WIDTH * _fontSize)
	  * _text.length(), MAX(_size, _size / 2 + _fontSize)));
}
