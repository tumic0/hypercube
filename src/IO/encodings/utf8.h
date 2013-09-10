#ifndef UTF8_H_
#define UTF8_H_

#include "IO/encoding.h"
#include "utf8cvt.h"

class UTF8 : public Encoding
{
public:
	virtual std::codecvt<wchar_t,char,mbstate_t>* cvt() const
	  {return new utf8cvt;}
	virtual const char *name() const {return "utf-8";}
};

#endif /* UTF8_H_ */
