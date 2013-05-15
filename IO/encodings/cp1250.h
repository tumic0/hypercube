#ifndef CP1250_H_
#define CP1250_H_

#include "IO/encoding.h"
#include "cp1250cvt.h"

class CP1250 : public Encoding
{
public:
	virtual std::codecvt<wchar_t,char,mbstate_t>* cvt() const
	  {return new cp1250cvt;}
	virtual const char *name() const {return "windows-1250";}
};

#endif /* CP1250_H_ */
