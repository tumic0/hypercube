#ifndef PS_LATIN1_H_
#define PS_LATIN1_H_

#include "IO/providers/ps_snippet.h"

class PsLatin1 : public PsEncoding
{
public:
	virtual const char *name() {return "iso8859-1";}
	virtual const char *encoding() {return 0;}
};

#endif /* PS_LATIN1_H_ */
