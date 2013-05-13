#ifndef ISO2_H_
#define ISO2_H_

#include "IO/encoding.h"
#include "iso2cvt.h"

class ISO2 : public Encoding
{
public:
	virtual std::codecvt<wchar_t,char,mbstate_t>* cvt() {return new iso2cvt;}
	virtual const char *name() {return "iso-8859-2";}
};

#endif /* ISO2_H_ */
