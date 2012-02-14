#ifndef COLORMAP_H_
#define COLORMAP_H_

#include <map>
#include <string>
#include "color.h"

class ColorMap
{
public:
	ColorMap();

	Color color(const std::wstring &str);
	void clear();

private:
	Color nextColor();

	std::map<std::wstring, Color> _colors;
	float _hueState;
};

#endif // COLORMAP_H_
