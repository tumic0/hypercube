#ifndef MARGIN_H_
#define MARGIN_H_

#include "coordinates.h"

class Margin
{
public:
	Margin() {_lt = Coordinates(0, 0); _rb = Coordinates(0, 0);}
	Margin(Coordinates lt, Coordinates rb)
	  {_lt = lt; _rb = rb;}

	Coordinates lt() const {return _lt;}
	Coordinates rb() const {return _rb;}
	void setLt(const Coordinates &lt) {_lt = lt;}
	void setRb(const Coordinates &rb) {_rb = rb;}

	bool operator==(const Margin &b)
	  {return (_rb == b._rb && _lt == b._lt);}
	bool operator!=(const Margin &b)
	  {return !(*this == b);}

	bool operator!() {return *this == Margin();}

	Margin& operator+=(const Margin &b)
	  {_lt += b._lt; _rb += b._rb; return *this;}
	Margin& operator-=(const Margin &b)
	  {_lt -= b._lt; _rb -= b._rb; return *this;}

	friend const Margin operator+(const Margin &a, const Margin &b)
	  {return Margin(a._lt + b._lt, a._rb + b._rb);}
	friend const Margin operator-(const Margin &a, const Margin &b)
	  {return Margin(a._lt - b._lt, a._rb - b._rb);}
	friend const Margin operator*(const Margin &a, int factor)
	  {return Margin(a._lt * factor, a._rb * factor);}
	friend const Margin operator/(const Margin &a, int divisor)
	  {return Margin(a._lt / divisor, a._rb / divisor);}

private:
	Coordinates _lt, _rb;
};


#endif /* MARGIN_H_ */
