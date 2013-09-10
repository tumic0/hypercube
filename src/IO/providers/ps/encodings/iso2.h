#ifndef PS_ISO2_H_
#define PS_ISO2_H_

#include "IO/providers/ps/snippet.h"
#include "iso2.ps.h"

class PsISO2 : public PsEncoding
{
public:
	virtual const char *name() const {return "iso-8859-2";}
	virtual const char *encoding() const {return iso2_ps;}
};

#endif /* PS_ISO2_H_ */
