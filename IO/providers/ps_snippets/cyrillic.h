#ifndef CYRILLIC_H_
#define CYRILLIC_H_

#include "IO/providers/ps_snippet.h"
#include "IO/providers/ps_snippets/encodings/koi8r.h"
#include "IO/providers/ps_snippets/fonts/nimbus.h"

class Cyrillic : public PsSnippet
{
	PsEncoding *_encoding;
	PsFont *_font;

public:
	Cyrillic() {_encoding = new PsKOI8R; _font = new Nimbus;}
	~Cyrillic() {delete _encoding; delete _font;}
	virtual PsEncoding *encoding() {return _encoding;}
	virtual PsFont *font() {return _font;}
};

#endif /* CYRILLIC_H_ */
