#ifndef DEFAULT_H_
#define DEFAULT_H_

#include "IO/providers/ps_snippet.h"
#include "IO/providers/ps_snippets/encodings/latin1.h"
#include "IO/providers/ps_snippets/fonts/helvetica.h"

class Default : public PsSnippet
{
	PsEncoding *_encoding;
	PsFont *_font;

public:
	Default() {_encoding = new PsLatin1; _font = new Helvetica;}
	~Default() {delete _encoding; delete _font;}
	virtual PsEncoding *encoding() {return _encoding;}
	virtual PsFont *font() {return _font;}
};

#endif /* DEFAULT_H_ */
