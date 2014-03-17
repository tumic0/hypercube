#ifndef LEGEND_H_
#define LEGEND_H_

#include "coordinates.h"
#include "colormap.h"

class Legend
{
public:
	Legend(const ColorMap *map);
	const Coordinates &dimensions() const {return _dimensions;}
	size_t size() const {return _size;}
	void setSize(size_t size);
	void updateMap();

	ColorMap::const_iterator begin() const
		{return _map->begin();}
	ColorMap::const_iterator end() const
		{return _map->end();}

private:
	void computeDimensions();

	const ColorMap *_map;
	size_t _size;
	Coordinates _dimensions;
};

#endif // LEGEND_H_
