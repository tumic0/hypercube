#ifndef COORDINATES_H_
#define COORDINATES_H_

#include <iostream>
#include <iomanip>
#include "misc.h"

class Coordinates
{
public:
	Coordinates() {_x = 0; _y = 0;}
	Coordinates(int x, int y) {_x = x; _y = y;}
	int x() {return _x;}
	int y() {return _y;}
	int& rx() {return _x;}
	int& ry() {return _y;}
	void setX(int x) {_x = x;}
	void setY(int y) {_y = y;}

	bool operator==(const Coordinates &b)
	  {return (_x == b._x && _y == b._y);}
	bool operator!=(const Coordinates &b)
	  {return !(*this == b);}

	bool operator!() {return *this == Coordinates();}

	Coordinates& operator+=(const Coordinates &b)
	  {_x += b._x; _y += b._y; return *this;}
	Coordinates& operator-=(const Coordinates &b)
	  {_x -= b._x; _y -= b._y; return *this;}

	friend const Coordinates operator+(const Coordinates &a,
	  const Coordinates &b) {return Coordinates(a._x + b._x, a._y + b._y);}
	friend const Coordinates operator-(const Coordinates &a,
	  const Coordinates &b) {return Coordinates(a._x - b._x, a._y - b._y);}
	friend const Coordinates operator*(const Coordinates &a, int factor)
	  {return Coordinates(a._x * factor, a._y * factor);}
	friend const Coordinates operator/(const Coordinates &a, int divisor)
	  {return Coordinates(a._x / divisor, a._y / divisor);}

	friend const Coordinates max(const Coordinates &a, const Coordinates &b)
	  {return Coordinates(MAX(a._x, b._x), MAX(a._y, b._y));}
	friend const Coordinates min(const Coordinates &a, const Coordinates &b)
	  {return Coordinates(MIN(a._x, b._x), MIN(a._y, b._y));}

	template< typename charT, typename Traits > friend
	  std::basic_ostream<charT, Traits>& operator<<(
	  std::basic_ostream<charT, Traits>& str, const Coordinates &co)
	{
		return str << co._x << (charT)',' << co._y;
	}
	template< typename charT, typename Traits > friend
	  std::basic_istream<charT, Traits>& operator>>(
	  std::basic_istream<charT, Traits>& str, Coordinates &co)
	{
		charT c;
		if (str >> co._x)
			if ((str >> c) && c == (charT)',')
				if (str >> co._y)
					return str;

		str.setstate(std::ios::failbit);
			return str;
	}

private:
	int _x, _y;
};

#endif /* COORDINATES_H_ */
