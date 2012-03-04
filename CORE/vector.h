#ifndef VECTOR_H_
#define VECTOR_H_

#include <vector>

template <class T> class Vector
{
public:
	~Vector() {
		for (size_t i = 0; i < _data.size(); i++)
			delete _data[i];
	}

	void clear() {
		for (size_t i = 0; i < _data.size(); i++)
			delete _data[i];
		_data.clear();
	}
	void push_back(const T& val) {_data.push_back(val);}
	size_t size() {return _data.size();}

	T& operator[](size_t pos) {return _data[pos];}

private:
	std::vector<T> _data;
};

#endif // ARRAY_H_
