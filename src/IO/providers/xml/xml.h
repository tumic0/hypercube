#ifndef XML_H_
#define XML_H_

#include <string>
#include "IO/io.h"
#include "IO/lexstream.h"


class XmlHandler
{
public:
	virtual bool startDocument() = 0;
	virtual bool endDocument() = 0;
	virtual bool startElement(const std::wstring &name) = 0;
	virtual bool endElement(const std::wstring &name) = 0;
	virtual bool attribute(const std::wstring &name,
	  const std::wstring &value) = 0;
	virtual bool data(const std::wstring &data) = 0;
};

class XmlParser : public IO
{
public:
	XmlParser(XmlHandler *handler) : _handler(handler) {}
	void setErrorPrefix(const std::string &prefix);
	IO::Error parse(const char *fileName);

private:
	enum Token {
		START,		/* Initial value */
		EOI,		/* End of File */
		ERROR,		/* Parse error */

		STRING,		/* Quoted string */
		IDENT,		/* Identificator (unknown) */
		DATA,		/* Character data */

		LT,			/* '<' */
		GT,			/* '>' */
		LSB,		/* '[' */
		RSB,		/* ']' */
		EQ,			/* '=' */
		SLASH,		/* '/' */
		QM,			/* '?' */
		EXCL,		/* '!' */
		MINUS		/* '-' */
	};

	void setEncoding(const std::wstring &encoding);
	void error();
	void nextToken();
	void compare(Token token);
	void data();
	void dtdData();
	void cdataData();
	void commentData();
	void comment();
	void cdata();
	void special();
	void attribute(bool xml);
	bool attributes(bool xml);
	void nextItem();
	void elementType();
	void element();
	void elementEnd();
	void prologSpecial();
	void prologContent();
	void prologComment();
	void dtdElement();
	void dtdElementType();
	void dtdMarkupDecl();
	void dtdExternalId();
	void dtdDecl();
	void xmlAttributes();
	void xmlDecl();
	void xml();
	void bom();
	bool start();

	std::wstring _string;
	lexstream _fs;
	Token _token;
	unsigned _line;
	std::string _errorPrefix;

	std::wstring _encoding;
	XmlHandler *_handler;
};

#endif // XML_H_
