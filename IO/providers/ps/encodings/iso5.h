#ifndef PS_ISO5_H_
#define PS_ISO5_H_

#include "IO/providers/ps/snippet.h"
#include "iso5.ps.h"

class PsISO5 : public PsEncoding
{
public:
	virtual const char *name() const {return "iso-8859-5";}
	virtual const char *encoding() const {return iso5_ps;}
};

#endif /* PS_ISO5_H_ */
