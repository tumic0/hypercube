#ifndef CYRILLIC_H_
#define CYRILLIC_H_

#include "snippet.h"
#include "encodings/koi8r.h"
#include "fonts/nimbus.h"

class Cyrillic : public PsSnippet
{
	PsEncoding *_encoding;
	PsFont *_font;

public:
	Cyrillic() {_encoding = new PsKOI8R; _font = new Nimbus;}
	virtual PsEncoding *encoding() const {return _encoding;}
	virtual PsFont *font() const {return _font;}
};

#endif /* CYRILLIC_H_ */
