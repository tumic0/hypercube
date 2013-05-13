#ifndef NIMBUS_H_
#define NIMBUS_H_

#include "IO/providers/ps/snippet.h"
#include "nimbus.pfa.h"

class Nimbus : public PsFont
{
public:
	virtual const char *name() const {return "NimbusSanL-Regu";}
	virtual const char *font() const {return nimbus_pfa;}
};

#endif /* NIMBUS_H_ */
