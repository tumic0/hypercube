#ifndef ISO1_H_
#define ISO1_H_

#include "IO/encoding.h"
#include "iso1cvt.h"

class ISO1 : public Encoding
{
public:
	virtual std::codecvt<wchar_t,char,mbstate_t>* cvt() {return new iso1cvt;}
	virtual const char *name() {return "iso-8859-1";}
};

#endif /* ISO1_H_ */
