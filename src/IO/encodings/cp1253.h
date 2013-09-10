#ifndef CP1253_H_
#define CP1253_H_

#include "IO/encoding.h"
#include "cp1253cvt.h"

class CP1253 : public Encoding
{
public:
	virtual std::codecvt<wchar_t,char,mbstate_t>* cvt() const
	  {return new cp1253cvt;}
	virtual const char *name() const {return "windows-1253";}
};

#endif /* CP1253_H_ */
