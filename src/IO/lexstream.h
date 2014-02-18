#ifndef LEXSTREAM_H_
#define LEXSTREAM_H_

#include <fstream>

class lexstream : public std::wifstream
{
public:
	lexstream() : _unget(false) {}
	void open(const char *filename, std::ios_base::openmode mode =
	  std::ios_base::in) {
		_unget = false;
		std::wifstream::open(filename, mode);
	}
	int get() {
		if (_unget) {
			if (is_open())
				_unget = false;
			else
				setstate(std::ios_base::badbit);
		} else
			_last = std::wifstream::get();
		return good() ? _last : -1;
	}
	void unget() {
		if (good() && is_open())
			_unget = true;
		else
			setstate(std::ios_base::badbit);
	}

private:
	int _last;
	bool _unget;
};

#endif // LEXSTREAM_H_
