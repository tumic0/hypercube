#ifndef ISO5_H_
#define ISO5_H_

#include "IO/encoding.h"
#include "iso5cvt.h"

class ISO5 : public Encoding
{
public:
	virtual std::codecvt<wchar_t,char,mbstate_t>* cvt() const
	  {return new iso5cvt;}
	virtual const char *name() const {return "iso-8859-5";}
};

#endif /* ISO5_H_ */
