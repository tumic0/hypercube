#ifndef HELVETICA_H_
#define HELVETICA_H_

#include "IO/providers/ps/snippet.h"

class Helvetica : public PsFont
{
public:
	virtual const char *name() const {return "Helvetica";}
	virtual const char *font() const {return 0;}
};

#endif /* HELVETICA_H_ */
