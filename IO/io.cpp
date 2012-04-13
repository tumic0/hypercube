#include <cmath>
#include "CORE/config.h"
#include "io.h"

using namespace std;

const float Pi = 3.141592f;
const float C1 = 0.866025f; /* sqrt(3)/2 */

CoordinatesF OutputProvider::edgeTextPosition(const LineF &line, float size,
  float fontSize, size_t textLength)
{
	float angle = abs(atan(line.dy() / line.dx()));
	float h = fontSize / 2;
	float w = (textLength * AVG_CHAR_WIDTH * fontSize) / 2;
	float hyp = sqrt(h*h + w*w);
	float dist = hyp * sin(angle + sin(h / hyp));

	LineF l(line);
	l.setP1(l.pointAt(0.5));
	l = l.normalVector();
	l.setLength((size / 2) + dist);

	return l.p2() - CoordinatesF(w, -h);
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
