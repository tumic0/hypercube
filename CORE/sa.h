#ifndef SA_H_
#define SA_H_

#include "graph.h"
#include "sa_log.h"

class SA
{
public:
	SA();
	void compute(Graph *graph);

	float nodeDistribution(void) const {return _nodeDistribution;};
	void setNodeDistribution(float value) {_nodeDistribution = value;};
	float edgeLength(void) const {return _edgeLength;};
	void setEdgeLength(float value) {_edgeLength = value;};
	float edgeCrossings(void) const {return _edgeCrossings;};
	void setEdgeCrossings(float value) {_edgeCrossings = value;};

	float initTemp(void) const {return _initTemp;};
	void setInitTemp(float value) {_initTemp = value;};
	float finalTemp(void) const {return _finalTemp;};
	void setFinalTemp(float value) {_finalTemp = value;};
	float coolFactor(void) const {return _coolFactor;};
	void setCoolFactor(float value) {_coolFactor = value;};
	int numSteps(void) const {return _numSteps;};
	void setNumSteps(int value) {_numSteps = value;};

#ifdef SA_LOG_SUPPORT
	bool logInfo(void) const {return _logInfo;};
	void setLogInfo(bool value) {_logInfo = value;};
#endif

private:
	float evaluateState(Graph *g);
	void newState(Graph *g, float temp);
	Coordinates newVertexLocation(Graph *g, int v, float temp);

	float _nodeDistribution, _edgeLength, _edgeCrossings;
	float _initTemp, _finalTemp, _coolFactor;
	int _numSteps;

#ifdef SA_LOG_SUPPORT
	bool _logInfo;
	std::ofstream _logFsProgress;
	std::ofstream _logFsOffset;
	std::ofstream _logFsAcceptance;
#endif
};

#endif /* SA_H_ */
