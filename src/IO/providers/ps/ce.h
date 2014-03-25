#ifndef CE_H_
#define CE_H_

#include "snippet.h"
#include "encodings/iso2.h"
#include "fonts/nimbus.h"

class CE : public PsSnippet
{
	PsEncoding *_encoding;
	PsFont *_font;

public:
	CE() {_encoding = new PsISO2; _font = new Nimbus;}
	virtual ~CE() {delete _encoding; delete _font;}
	virtual PsEncoding *encoding() const {return _encoding;}
	virtual PsFont *font() const {return _font;}
};

#endif /* CE_H_ */
