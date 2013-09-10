#ifndef COLORMAP_H_
#define COLORMAP_H_

#include <map>
#include <string>
#include "color.h"

class ColorMap
{
public:
	class iterator {
	public:
		iterator() {}
		iterator(std::map<std::wstring, Color>::iterator it)
		  : _it(it) {}

		iterator& operator=(const iterator& other)
			{_it = other._it; return(*this);}
		bool operator==(const iterator& other)
			{return(_it == other._it);}
		bool operator!=(const iterator& other)
			{return(_it != other._it);}
		std::pair<std::wstring, Color> operator*()
			{return *_it;}
		iterator operator++(int)
			{iterator tmp(*this); _it++; return tmp;}
	private:
		std::map<std::wstring, Color>::iterator _it;
	};

	ColorMap();
	Color color(const std::wstring &str);
	void clear();

	iterator begin()
		{return iterator(_colors.begin());}
	iterator end()
		{return iterator(_colors.end());}

private:
	Color nextColor();

	std::map<std::wstring, Color> _colors;
	float _hueState;
};

#endif // COLORMAP_H_
