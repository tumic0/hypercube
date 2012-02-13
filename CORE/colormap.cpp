#include "colormap.h"

#define HUE_INIT		0.1f
#define HUE_INCREMENT	0.62f
#define SATURATION		0.99f
#define VALUE			0.99f

using namespace std;

static unsigned hsv2rgb(float h, float s, float v);


ColorMap::ColorMap()
{
	_hueState = HUE_INIT;
}

Color ColorMap::color(const std::wstring &str)
{
	map<wstring, Color>::iterator it;
	Color color;

	it = _colors.find(str);

	if (it == _colors.end()) {
		color = Color(nextColor());
		_colors.insert(pair<wstring, Color>(str, color));
	} else {
		color = (*it).second;
	}

	return color;
}

void ColorMap::clear()
{
	_hueState = HUE_INIT;
	_colors.clear();
}

Color ColorMap::nextColor()
{
	_hueState += HUE_INCREMENT;
	_hueState -= (int) _hueState;
	return Color(hsv2rgb(_hueState, SATURATION, VALUE));
}

unsigned hsv2rgb(float h, float s, float v)
{
	unsigned hi;
	float r = 0, g = 0, b = 0, p, q, t, f;

	hi = (unsigned)(h * 6.0f);
	f = h * 6.0f - hi;
	p = v * (1.0f - s);
	q = v * (1.0f - f * s);
	t = v * (1.0f - (1.0f - f) * s);

	switch (hi) {
		case 0:
			r = v; g = t; b = p;
			break;
		case 1:
			r = q; g = v; b = p;
			break;
		case 2:
			r = p; g = v; b = t;
			break;
		case 3:
			r = p; g = q; b = v;
			break;
		case 4:
			r = t; g = p; b = v;
			break;
		case 5:
			r = v; g = p; b = q;
			break;
	}

	return ((unsigned)(r * 256) << 16)
	  + ((unsigned)(g * 256) << 8)
	  + (unsigned)(b * 256);
}
