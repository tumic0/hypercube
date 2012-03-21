#include <cstdlib>
#include <cmath>
#include "misc.h"
#include "line.h"


int Line::lengthSqr() const
{
	int x = _p2.x() - _p1.x();
	int y = _p2.y() - _p1.y();

	return abs(x*x + y*y);
}

bool Line::crossing(const Line &line) const
{
	int ua_t, ub_t, u_b;

	ua_t = (line.p2().x() - line.p1().x()) * (_p1.y() - line.p1().y())
	  - (line.p2().y() - line.p1().y()) * (_p1.x() - line.p1().x());
	ub_t = (_p2.x() - _p1.x()) * (_p1.y() - line.p1().y())
	  - (_p2.y() - _p1.y()) * (_p1.x() - line.p1().x());
	u_b = (line.p2().y() - line.p1().y()) * (_p2.x() - _p1.x())
	  - (line.p2().x() - line.p1().x()) * (_p2.y() - _p1.y());

	if (u_b == 0) {
		if (ua_t == 0 && ub_t == 0) {
			int lx = MIN(_p1.x(), _p1.x() + (_p2.x() - _p1.x()));
			int ux = MAX(_p1.x(), _p1.x() + (_p2.x() - _p1.x()));
			int ly = MIN(_p1.y(), _p1.y() + (_p2.y() - _p1.y()));
			int uy = MAX(_p1.y(), _p1.y() + (_p2.y() - _p1.y()));

			if (((line.p1().x() >= lx && line.p1().x() <= ux)
			  || (line.p2().x() >= lx && line.p2().x() <= ux))
			  && ((line.p1().y() >= ly && line.p1().y() <= uy)
			  || (line.p2().y() >= ly && line.p2().y() <= uy)))
				return true;
		}
	} else {
		float ua = (float)ua_t / (float)u_b;
		float ub = (float)ub_t / (float)u_b;

		if (0 <= ua && ua <= 1 && 0 <= ub && ub <= 1)
			return true;
	}

	return false;
}


float LineF::length() const
{
	float x = _p2.x() - _p1.x();
	float y = _p2.y() - _p1.y();

	return sqrt(x*x + y*y);
}

CoordinatesF LineF::pointAt(float t) const
{
	float x = _p2.x() - _p1.x();
	float y = _p2.y() - _p1.y();

	return CoordinatesF(_p1.x() + x * t, _p1.y() + y * t);
}
