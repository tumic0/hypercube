#ifndef PS_SNIPPET_H_
#define PS_SNIPPET_H_

class PsFont
{
public:
	virtual const char *name() = 0;
	virtual const char *font() = 0;
};

class PsEncoding
{
public:
	virtual const char *name() = 0;
	virtual const char *encoding() = 0;
};

class PsSnippet
{
public:
	virtual PsFont *font() = 0;
	virtual PsEncoding *encoding() = 0;
};

extern PsSnippet* snippets[];

#endif /* PS_SNIPPET_H_ */
