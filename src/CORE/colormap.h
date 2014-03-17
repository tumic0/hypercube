#ifndef COLORMAP_H_
#define COLORMAP_H_

#include <map>
#include <string>
#include "color.h"

class ColorMap
{
public:
	class const_iterator {
	public:
		const_iterator() {}
		const_iterator(std::map<std::wstring, Color>::const_iterator it)
		  : _it(it) {}

		const_iterator& operator=(const const_iterator& other)
			{_it = other._it; return(*this);}
		bool operator==(const const_iterator& other)
			{return(_it == other._it);}
		bool operator!=(const const_iterator& other)
			{return(_it != other._it);}
		std::pair<std::wstring, Color> operator*()
			{return *_it;}
		const_iterator operator++(int)
			{const_iterator tmp(*this); _it++; return tmp;}
	private:
		std::map<std::wstring, Color>::const_iterator _it;
	};

	ColorMap();
	Color color(const std::wstring &str);
	void clear();
	bool empty() const {return _colors.empty();}

	const_iterator begin() const
		{return const_iterator(_colors.begin());}
	const_iterator end() const
		{return const_iterator(_colors.end());}

private:
	Color nextColor();

	std::map<std::wstring, Color> _colors;
	float _hueState;
};

#endif // COLORMAP_H_
