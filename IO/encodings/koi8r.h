#ifndef KOI8R_H_
#define KOI8R_H_

#include "IO/encoding.h"
#include "koi8rcvt.h"

class KOI8R : public Encoding
{
public:
	virtual std::codecvt<wchar_t,char,mbstate_t>* cvt() {return new koi8rcvt;}
	virtual const char *name() {return "koi8-r";}
};

#endif /* KOI8R_H_ */
