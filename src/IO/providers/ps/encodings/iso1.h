#ifndef PS_ISO1_H_
#define PS_ISO1_H_

#include "IO/providers/ps/snippet.h"

class PsISO1 : public PsEncoding
{
public:
	virtual const char *name() const {return "iso-8859-1";}
	virtual const char *encoding() const {return 0;}
};

#endif /* PS_ISO1_H_ */
