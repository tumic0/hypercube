#ifndef WESTERN_H_
#define WESTERN_H_

#include "snippet.h"
#include "encodings/iso1.h"
#include "fonts/helvetica.h"

class Western : public PsSnippet
{
	PsEncoding *_encoding;
	PsFont *_font;

public:
	Western() {_encoding = new PsISO1; _font = new Helvetica;}
	virtual ~Western() {delete _encoding; delete _font;}
	virtual PsEncoding *encoding() const {return _encoding;}
	virtual PsFont *font() const {return _font;}
};

#endif /* WESTERN_H_ */
