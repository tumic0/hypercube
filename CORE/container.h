#ifndef CONTAINER_H_
#define CONTAINER_H_

#include <vector>

template <class T> class LinearContainer
{
public:
	LinearContainer() : _sum(T()) {}

	void increment() {_data.push_back(T());}
	void clear() {_data.clear(); _sum = T();}

	void setValue(size_t id, T val)
	{
		_sum -= _data[id];
		_data[id] = val;
		_sum += val;
	}

	T sum() const {return _sum;}

private:
	std::vector<T> _data;
	T _sum;
};


template <class T> class MatrixContainer
{
public:
	MatrixContainer() : _sum(T()) {}

	void increment()
	{
		_data.push_back(std::vector<T>(_data.size() + 1, T()));
	}

	void clear()
	{
		for (size_t i = 0; i < _data.size(); i++)
			_data[i].clear();
		_data.clear();
		_sum = T();
	}

	void setValue(size_t id1, size_t id2, T val)
	{
		size_t i, j;

		if (id1 > id2) {
			i = id1;
			j = id2;
		} else {
			i = id2;
			j = id1;
		}

		_sum -= _data[i][j];
		_data[i][j] = val;
		_sum += val;
	}

	T sum() const {return _sum;}

private:
	std::vector<std::vector<T> > _data;
	T _sum;
};

#endif // CONTAINER_H_
