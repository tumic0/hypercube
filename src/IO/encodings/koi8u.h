#ifndef KOI8U_H_
#define KOI8U_H_

#include "IO/encoding.h"
#include "koi8ucvt.h"

class KOI8U : public Encoding
{
public:
	virtual std::codecvt<wchar_t,char,mbstate_t>* cvt() const
	  {return new koi8ucvt;}
	virtual const char *name() const {return "koi8-u";}
};

#endif /* KOI8U_H_ */
