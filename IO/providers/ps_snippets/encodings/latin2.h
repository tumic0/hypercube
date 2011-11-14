#ifndef PS_LATIN2_H_
#define PS_LATIN2_H_

#include "IO/providers/ps_snippet.h"
#include "latin2.ps.h"

class PsLatin2 : public PsEncoding
{
public:
	virtual const char *name() {return "iso8859-2";}
	virtual const char *encoding() {return latin2_ps;}
};

#endif /* PS_LATIN2_H_ */
