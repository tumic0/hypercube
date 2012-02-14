#ifndef CE_H_
#define CE_H_

#include "snippet.h"
#include "encodings/latin2.h"
#include "fonts/helvetica_ce.h"

class CE : public PsSnippet
{
	PsEncoding *_encoding;
	PsFont *_font;

public:
	CE() {_encoding = new PsLatin2; _font = new HelveticaCE;}
	~CE() {delete _encoding; delete _font;}
	virtual PsEncoding *encoding() {return _encoding;}
	virtual PsFont *font() {return _font;}
};

#endif /* CE_H_ */
