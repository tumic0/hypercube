#ifndef GREEK_H_
#define GREEK_H_

#include "snippet.h"
#include "encodings/iso7.h"
#include "fonts/moderna.h"

class Greek : public PsSnippet
{
	PsEncoding *_encoding;
	PsFont *_font;

public:
	Greek() {_encoding = new PsISO7; _font = new Moderna;}
	virtual ~Greek() {delete _encoding; delete _font;}
	virtual PsEncoding *encoding() const {return _encoding;}
	virtual PsFont *font() const {return _font;}
};

#endif /* GREEK_H_ */
