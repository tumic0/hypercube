#include "misc.h"
#include "config.h"
#include "legend.h"

Legend::Legend(const ColorMap *map)
{
	_map = map;
	_size = 0;
}

void Legend::setSize(int size)
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
	int height = LEGEND_MARGIN, tl = 0;
	float width;

	if (!_size) {
		_dimensions = Coordinates();
		return;
	}

	for (ColorMap::const_iterator it = _map->begin(); it != _map->end(); it++) {
		tl = MAX(tl, (int)((*it).first.length()));
		height += _size * LEGEND_RECT_RATIO;
	}

	width = LEGEND_MARGIN + (_size * LEGEND_RECT_RATIO) + (_size / 3)
	  + (_size * AVG_CHAR_WIDTH * tl);

	_dimensions = Coordinates((int)width, height);
}
