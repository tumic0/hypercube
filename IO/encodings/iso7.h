#ifndef ISO7_H_
#define ISO7_H_

#include "IO/encoding.h"
#include "iso7cvt.h"

class ISO7 : public Encoding
{
public:
	virtual std::codecvt<wchar_t,char,mbstate_t>* cvt() const
	  {return new iso7cvt;}
	virtual const char *name() const {return "iso-8859-7";}
};

#endif /* ISO7_H_ */
