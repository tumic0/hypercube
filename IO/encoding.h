#ifndef ENCODING_H_
#define ENCODING_H_

#include <locale>

class Encoding
{
public:
	virtual std::codecvt<wchar_t,char,mbstate_t>* cvt() = 0;
	virtual const char *name() = 0;
};

#endif /* ENCODING_H_ */
