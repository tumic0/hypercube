#ifndef PS_KOI8R_H_
#define PS_KOI8R_H_

#include "IO/providers/ps/snippet.h"
#include "koi8r.ps.h"

class PsKOI8R : public PsEncoding
{
public:
	virtual const char *name() const {return "koi8-r";}
	virtual const char *encoding() const {return koi8r_ps;}
};

#endif /* PS_KOI8R_H_ */
