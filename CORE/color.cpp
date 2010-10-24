#include "color.h"

#define RGB_MASK 0x00FFFFFF

/*!
	\class Color
	\brief The Color class provides colors based on RGB.

	A 24-bit representation (8 bits per channel) is used to represent the
	color. No transparency is supported.
*/


/*!
	Constructs an invalid color.

	The RGB value of an invalid color is undefined.

	\sa valid()
*/
Color::Color()
{
	_data = ~RGB_MASK;
}

/*!
	Constructs a color with RGB value \a rgb.
*/
Color::Color(unsigned rgb)
{
	_data = rgb & RGB_MASK;
}

/*!
	Returns true if the color is valid; otherwise returns false.
*/
bool Color::valid()
{
	return _data & ~RGB_MASK;
}

/*!
	Returns the RGB value of the color.
*/
unsigned Color::rgb()
{
	return _data;
}

/*!
	Returns the red color component of the color.
*/
float Color::red()
{
	return ((float)(_data >> 16) / 255);
}

/*!
	Returns the green color component of the color.
*/
float Color::green()
{
	return ((float)((_data & 0x0000FF00) >> 8) / 255);
}

/*!
	Returns the blue color component of the color.
*/
float Color::blue()
{
	return ((float)(_data & 0x000000FF) / 255);
}

/*!
	\fn Color::operator<< \
	  (std::basic_ostream< charT, Traits > &str, const Color &color)

	Writes the color to the stream.

	The output format is \#RRGGBB as used in HTML or SVG.
*/

/*!
	\fn Color::operator>> \
	  (std::basic_istream< charT, Traits > &str, Color &color)

	Reads the color from the stream.

	The input format is \#RRGGBB as used in HTML or SVG.
*/

/*!
	\fn bool Color::operator==(const Color &color)

	Returns true if the color has the same RGB value as \a color, otherwise
	returns false.

	If one of the colors is invalid, returns false, if both are invalid,
	returns true.
*/

/*!
	\fn bool Color::operator!=(const Color &color)

	Returns true if the color has a different RGB value then \a color,
	otherwise returns false.
*/
