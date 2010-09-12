#ifndef COLOR_H_
#define COLOR_H_

#include <iostream>
#include <iomanip>

class Color
{
public:
	Color();
	Color(unsigned rgb);

	bool valid();
	unsigned rgb();

	float red();
	float green();
	float blue();

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
	unsigned _data;
};

#endif /* COLOR_H_ */
