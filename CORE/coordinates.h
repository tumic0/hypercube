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
	Coordinates(const Coordinates &c) {_x = c.x(), _y = c.y();}

	int x() const {return _x;}
	int y() const {return _y;}
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


class CoordinatesF
{
public:
	CoordinatesF() {_x = 0; _y = 0;}
	CoordinatesF(float x, float y) {_x = x; _y = y;}
	CoordinatesF(const CoordinatesF &c) {_x = c.x(), _y = c.y();}
	CoordinatesF(const Coordinates &c) {_x = (float)c.x(); _y = (float)c.y();}

	float x() const {return _x;}
	float y() const {return _y;}
	void setX(float x) {_x = x;}
	void setY(float y) {_y = y;}

	CoordinatesF& operator+=(const CoordinatesF &b)
	  {_x += b._x; _y += b._y; return *this;}
	CoordinatesF& operator-=(const CoordinatesF &b)
	  {_x -= b._x; _y -= b._y; return *this;}

	friend const CoordinatesF operator+(const CoordinatesF &a,
	  const CoordinatesF &b) {return CoordinatesF(a._x + b._x, a._y + b._y);}
	friend const CoordinatesF operator-(const CoordinatesF &a,
	  const CoordinatesF &b) {return CoordinatesF(a._x - b._x, a._y - b._y);}
	friend const CoordinatesF operator*(const CoordinatesF &a, float factor)
	  {return CoordinatesF(a._x * factor, a._y * factor);}
	friend const CoordinatesF operator/(const CoordinatesF &a, float divisor)
	  {return CoordinatesF(a._x / divisor, a._y / divisor);}

	Coordinates toCoordinates()
	  {return Coordinates(round(_x), round(_y));}

private:
	float _x, _y;

	int round(float r)
	  {return (r > 0.0) ? (int)(r + 0.5) : (int)(r - 0.5);}
};

#endif /* COORDINATES_H_ */
