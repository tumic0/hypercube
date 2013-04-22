#ifndef LATIN1_H_
#define LATIN1_H_

#include "IO/encoding.h"
#include "latin1cvt.h"

class Latin1 : public Encoding
{
public:
	virtual std::codecvt<wchar_t,char,mbstate_t>* cvt() {return new latin1cvt;}
	virtual const char *name() {return "iso-8859-1";}
};

#endif /* LATIN1_H_ */
