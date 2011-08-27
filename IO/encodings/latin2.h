#ifndef LATIN2_H_
#define LATIN2_H_

#include "IO/encoding.h"
#include "latin2cvt.h"

class Latin2 : public Encoding
{
public:
	virtual std::codecvt<wchar_t,char,mbstate_t>* cvt() {return new latin2cvt;}
	virtual const char *name() {return "iso8859-2";}
};

#endif /* LATIN2_H_ */
