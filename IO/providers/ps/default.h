#ifndef DEFAULT_H_
#define DEFAULT_H_

#include "snippet.h"
#include "encodings/latin1.h"
#include "fonts/helvetica.h"

class Default : public PsSnippet
{
	PsEncoding *_encoding;
	PsFont *_font;

public:
	Default() {_encoding = new PsLatin1; _font = new Helvetica;}
	virtual PsEncoding *encoding() {return _encoding;}
	virtual PsFont *font() {return _font;}
};

#endif /* DEFAULT_H_ */
