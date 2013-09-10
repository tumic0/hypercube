#ifndef MODERNA_H_
#define MODERNA_H_

#include "IO/providers/ps/snippet.h"
#include "moderna.pfa.h"

class Moderna : public PsFont
{
public:
	virtual const char *name() const {return "MgOpenModerna";}
	virtual const char *font() const {return moderna_pfa;}
};

#endif /* MODERNA_H_ */
