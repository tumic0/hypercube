#ifndef NIMBUS_H_
#define NIMBUS_H_

#include "IO/providers/ps_snippet.h"
#include "nimbus.pfa.h"

class Nimbus : public PsFont
{
public:
	virtual const char *name() {return "NimbusSanL-Regu";}
	virtual const char *font() {return nimbus_pfa;}
};

#endif /* NIMBUS_H_ */
