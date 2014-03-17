#include "misc.h"
#include "config.h"
#include "legend.h"

Legend::Legend(const ColorMap *map)
{
	_map = map;
	_size = 0;
}

void Legend::setSize(size_t size)
{
	_size = size;
	computeDimensions();
}

void Legend::updateMap()
{
	computeDimensions();
}

void Legend::computeDimensions()
{
	size_t height = LEGEND_MARGIN, tl = 0;
	float width;

	if (!_size) {
		_dimensions = Coordinates();
		return;
	}

	for (ColorMap::const_iterator it = _map->begin(); it != _map->end(); it++) {
		tl = MAX(tl, (*it).first.length());
		height += _size * LEGEND_RECT_RATIO;
	}

	width = LEGEND_MARGIN + (_size * LEGEND_RECT_RATIO) + (_size / 3)
	  + (_size * AVG_CHAR_WIDTH * tl);

	_dimensions = Coordinates((size_t)width, height);
}
