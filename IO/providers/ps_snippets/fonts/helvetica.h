#ifndef HELVETICA_H_
#define HELVETICA_H_

#include "IO/providers/ps_snippet.h"

class Helvetica : public PsFont
{
public:
	virtual const char *name() {return "Helvetica";}
	virtual const char *font() {return 0;}
};

#endif /* HELVETICA_H_ */
