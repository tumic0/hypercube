#ifndef CP1252_H_
#define CP1252_H_

#include "IO/encoding.h"
#include "cp1252cvt.h"

class CP1252 : public Encoding
{
public:
	virtual std::codecvt<wchar_t,char,mbstate_t>* cvt() const
	  {return new cp1252cvt;}
	virtual const char *name() const {return "windows-1252";}
};

#endif /* CP1252_H_ */
