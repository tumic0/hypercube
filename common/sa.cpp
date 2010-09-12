#include <cstdlib>
#include <cmath>
#include <ctime>
#include "graph.h"
#include "sa.h"
#include "sa_log.h"


/*!
	\class SA
	\brief Simulated annealing computation

	Lorem ipsum...
*/

static inline float rnd();
static inline int crop(int x, int min, int max);


SA::SA()
{
	_nodeDistribution = 8000000;
	_edgeLength = 4;
	_edgeCrossings = 100000;

	_initTemp = 10000;
	_finalTemp = 100;
	_coolFactor = 0.75;
	_numSteps = 800;

	LOG_INIT();
}

inline Coordinates SA::newVertexLocation(Graph *g, int v, float temp)
{
	Coordinates offset, location;
	Coordinates bounds = g->dimensions();
	Coordinates loc = g->vertexCoordinates(v);
	Margin margin = g->margin(v);

	offset.setX((int)((temp / _initTemp) * bounds.x() * (2 * rnd() - 1)));
	offset.setY((int)((temp / _initTemp) * bounds.y() * (2 * rnd() - 1)));
	location.setX(crop(loc.x() + offset.x(), margin.lt().x(),
	  bounds.x() - 1 - margin.rb().x()));
	location.setY(crop(loc.y() + offset.y(), margin.lt().y(),
	  bounds.y() - 1 - margin.rb().y()));
	LOG_OFFSET(offset);

	return location;
}

inline float SA::evaluateState(Graph *g)
{
	return
	  _nodeDistribution * g->distance()
	  + _edgeLength * g->length()
	  + _edgeCrossings * g->crossings();
}

inline void SA::newState(Graph *g, float temp)
{
	int id;
	float cost, new_cost, delta, ex, acc;

	cost = evaluateState(g);
	id = rand() % g->size();
	Coordinates location = g->vertexCoordinates(id);
	Coordinates new_location = newVertexLocation(g, id, temp);
	g->moveVertex(id, new_location);
	new_cost = evaluateState(g);

	delta = (new_cost - cost);
	ex = rnd();
	acc = exp(-delta / temp);
	if ((delta < 0) || (ex < acc))
		cost = new_cost;
	else
		g->moveVertex(id, location);

	LOG_ACCEPTANCE(delta, ex, acc);
}

void SA::compute(Graph *g)
{
	float temp = _initTemp;
	int steps;

	LOG_START();

	do {
		steps = _numSteps;
		do {
			newState(g, temp);
			steps--;
			LOG_PROGRESS(g);
		} while (steps);
		temp = temp * _coolFactor;
	} while (temp > _finalTemp);

	LOG_END();
}


static inline float rnd()
{
	return ((float) rand() / (float)RAND_MAX);
}

static inline int crop(int x, int min, int max)
{
	int retval;

	if (x < min)
		retval = min;
	else if (x > max)
		retval = max;
	else
		retval = x;

	return retval;
}
