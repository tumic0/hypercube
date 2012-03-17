#include <cmath>
#include "io.h"


const float Pi = 3.141592f;
const float C1 = 0.866025f; /* sqrt(3)/2 */

CoordinatesF OutputProvider::edgeTextPosition(const LineF &line, float size,
  float fontSize)
{
	if ((line.dx() > 0 && line.dy() > 0) || (line.dx() < 0 && line.dy() < 0))
		return CoordinatesF(size / 2, -size / 2) + line.pointAt(0.5);
	else
		return CoordinatesF(size / 2, size / 2 + fontSize)
		  + line.pointAt(0.5);
}

CoordinatesF OutputProvider::vertexTextPosition(const CoordinatesF &point,
  float size)
{
	return point + CoordinatesF(size / 2, size);
}

OutputProvider::Arrow OutputProvider::arrow(LineF &line, float size)
{
	Arrow arrow;
	float angle;

	angle = acos(line.dx() / line.length());
	if (line.dy() >= 0)
		angle = 2 * Pi - angle;

	arrow.p[0] = line.pointAt(1 - ((size / 2) / line.length()));
	arrow.p[1] = CoordinatesF(sin(angle - Pi / 3) * size,
	  cos(angle - Pi / 3) * size) + arrow.p[0];
	arrow.p[2] = CoordinatesF(sin(angle - Pi + Pi / 3) * size,
	  cos(angle - Pi + Pi / 3) * size) + arrow.p[0];

	line.setP2(
	  line.pointAt(1 - ((size * C1 * 1.5f) / line.length())));

	return arrow;
}
