#ifndef PS_ISO7_H_
#define PS_ISO7_H_

#include "IO/providers/ps/snippet.h"
#include "iso7.ps.h"

class PsISO7 : public PsEncoding
{
public:
	virtual const char *name() const {return "iso-8859-7";}
	virtual const char *encoding() const {return iso7_ps;}
};

#endif /* PS_ISO7_H_ */
