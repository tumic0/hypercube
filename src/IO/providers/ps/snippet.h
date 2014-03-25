#ifndef PS_SNIPPET_H_
#define PS_SNIPPET_H_

class PsFont
{
public:
	virtual ~PsFont() {}
	virtual const char *name() const = 0;
	virtual const char *font() const = 0;
};

class PsEncoding
{
public:
	virtual ~PsEncoding() {}
	virtual const char *name() const = 0;
	virtual const char *encoding() const = 0;
};

class PsSnippet
{
public:
	virtual ~PsSnippet() {}
	virtual PsFont *font() const = 0;
	virtual PsEncoding *encoding() const = 0;
};

extern PsSnippet* snippets[];

#endif /* PS_SNIPPET_H_ */
