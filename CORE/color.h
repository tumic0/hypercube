#ifndef COLOR_H_
#define COLOR_H_

#include <iostream>
#include <iomanip>


class Color
{
public:
	Color() {_data = ~RGB_MASK;}
	Color(unsigned rgb) {_data = rgb & RGB_MASK;}

	bool valid() {return (_data & ~RGB_MASK) ? true : false;}
	unsigned rgb() {return _data;}

	float red() {return ((float)(_data >> 16) / 255);}
	float green() {return ((float)((_data & 0x0000FF00) >> 8) / 255);}
	float blue() {return ((float)(_data & 0x000000FF) / 255);}

	bool operator==(const Color &color) {return (_data == color._data);}
	bool operator!=(const Color &color) {return !(*this == color);}

	template< typename charT, typename Traits > friend
	  std::basic_ostream<charT, Traits>& operator<<(
	  std::basic_ostream<charT, Traits>& str, const Color &color)
	{
		return str << (charT)'#' << std::hex << std::setfill((charT)'0')
		  << std::setw(6) << color._data << std::dec;
	}
	template< typename charT, typename Traits > friend
	  std::basic_istream<charT, Traits>& operator>>(
	  std::basic_istream<charT, Traits>& str, Color &color)
	{
		charT c;
		if ((str >> c) && c == (charT)'#')
			if (str >> std::hex >> color._data)
				return str;

		str.setstate(std::ios::failbit);
		return str;
	}

private:
	static const unsigned RGB_MASK = 0x00FFFFFF;
	unsigned _data;
};

#endif /* COLOR_H_ */
