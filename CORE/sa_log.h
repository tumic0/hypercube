#ifndef SA_LOG_H_
#define SA_LOG_H_

#include "config.h"

#ifdef SA_LOG_SUPPORT

#include <stdexcept>
#include <iostream>
#include <fstream>


#define LOG_INIT() \
	_logInfo = false;

#define LOG_START() \
	if (_logInfo) { \
		_logFsProgress.open(SA_PROGRESS_LOG); \
		_logFsOffset.open(SA_OFFSET_LOG); \
		_logFsAcceptance.open(SA_ACCEPTANCE_LOG); \
		if (!_logFsProgress || !_logFsOffset || !_logFsAcceptance) \
			throw std::logic_error("Error opening log file!"); \
	}

#define LOG_END() \
	if (_logInfo) { \
		_logFsProgress.close(); \
		_logFsOffset.close(); \
		_logFsAcceptance.close(); \
	}

#define LOG_PROGRESS(graph) \
	if (_logInfo) { \
		_logFsProgress << evaluateState(graph) << " "; \
		_logFsProgress << _nodeDistribution * graph->distance() << " "; \
		_logFsProgress << _edgeLength * graph->length() << " "; \
		_logFsProgress << _edgeCrossings * graph->crossings() << std::endl; \
	}

#define LOG_OFFSET(offset) \
	if (_logInfo) \
		_logFsOffset << offset.x() << " " << offset.y() << std::endl;

#define LOG_ACCEPTANCE(delta, rnd, cmp) \
	if (_logInfo) \
		if (delta > 0) \
			_logFsAcceptance << rnd << " " << cmp << std::endl;

#else /* SA_LOG_SUPPORT */

#define LOG_INIT()
#define LOG_START()
#define LOG_END()
#define LOG_PROGRESS(graph)
#define LOG_OFFSET(offset)
#define LOG_ACCEPTANCE(delta, rnd, cmp)

#endif /* SA_LOG_SUPPORT */

#endif /* SA_LOG_H_ */
