#ifndef ADJLIST_H_
#define ADJLIST_H_

#include <vector>
#include "vertex.h"
#include "edge.h"

class AdjacencyList
{
public:

	class edge_iterator {
	public:
		edge_iterator() {}
		edge_iterator(std::vector<size_t>::iterator it) : _it(it) {}

		bool operator==(const edge_iterator& other)
			{return(_it == other._it);}
		bool operator!=(const edge_iterator& other)
			{return(_it != other._it);}
		size_t operator*()
			{return *_it;}
		edge_iterator operator++(int)
			{edge_iterator tmp(*this); _it++; return tmp;}
	private:
		std::vector<size_t>::iterator _it;
	};

	void addVertex()
		{_data.push_back(std::vector<size_t>());}
	void addEdge(Edge *edge)
	{
		_data[edge->src()->id()].push_back(edge->id());
		_data[edge->dst()->id()].push_back(edge->id());
	}

	void clear()
		{_data.clear();}

	edge_iterator begin(size_t vertex)
		{return edge_iterator(_data[vertex].begin());}
	edge_iterator end(size_t vertex)
		{return edge_iterator(_data[vertex].end());}

private:
	std::vector<std::vector<size_t> > _data;
};

#endif // ADJLIST_H_
