#ifndef CP1251_H_
#define CP1251_H_

#include "IO/encoding.h"
#include "cp1251cvt.h"

class CP1251 : public Encoding
{
public:
	virtual std::codecvt<wchar_t,char,mbstate_t>* cvt() const
	  {return new cp1251cvt;}
	virtual const char *name() const {return "windows-1251";}
};

#endif /* CP1251_H_ */
