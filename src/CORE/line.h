#ifndef LINE_H_
#define LINE_H_

#include <cmath>
#include "coordinates.h"

class Line
{
public:
	Line() : _p1(), _p2() {}
	Line(const Coordinates &p1, const Coordinates &p2) : _p1(p1), _p2(p2) {}

	Coordinates p1() const {return _p1;}
	Coordinates p2() const {return _p2;}
	void setP1(const Coordinates &p1) {_p1 = p1;}
	void setP2(const Coordinates &p2) {_p2 = p2;}

	int dx() const {return _p2.x() - _p1.x();}
	int dy() const {return _p2.y() - _p1.y();}

	bool crossing(const Line &line) const;
	int lengthSqr() const {return (dx() * dx() + dy() * dy());}

private:
	Coordinates _p1, _p2;
};


class LineF
{
public:
	LineF() : _p1(), _p2() {}
	LineF(const CoordinatesF &p1, const CoordinatesF &p2) : _p1(p1), _p2(p2) {}

	CoordinatesF p1() const {return _p1;}
	CoordinatesF p2() const {return _p2;}
	void setP1(const CoordinatesF &p1) {_p1 = p1;}
	void setP2(const CoordinatesF &p2) {_p2 = p2;}

	float dx() const {return _p2.x() - _p1.x();}
	float dy() const {return _p2.y() - _p1.y();}

	LineF unitVector() const
	  {return LineF(_p1, CoordinatesF(_p1.x() + dx() / length(),
	   _p1.y() + dy() / length()));}
	LineF normalVector() const
	  {return LineF(p1(), p1() + CoordinatesF(dy(), -dx()));}
	float length() const
	  {return sqrt(dx() * dx() + dy() * dy());}

	CoordinatesF pointAt(float t) const
	  {return CoordinatesF(_p1.x() + dx() * t, _p1.y() + dy() * t);}

	void setLength(float length);
private:
	CoordinatesF _p1, _p2;
};

#endif // LINE_H_
