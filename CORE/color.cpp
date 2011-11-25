#include "color.h"


#define RGB_MASK 0x00FFFFFF

Color::Color()
{
	_data = ~RGB_MASK;
}

Color::Color(unsigned rgb)
{
	_data = rgb & RGB_MASK;
}

bool Color::valid()
{
	return _data & ~RGB_MASK;
}

unsigned Color::rgb()
{
	return _data;
}

float Color::red()
{
	return ((float)(_data >> 16) / 255);
}

float Color::green()
{
	return ((float)((_data & 0x0000FF00) >> 8) / 255);
}

float Color::blue()
{
	return ((float)(_data & 0x000000FF) / 255);
}
