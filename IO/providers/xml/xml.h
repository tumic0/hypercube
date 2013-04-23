#ifndef XML_H_
#define XML_H_


#include <string>
#include <fstream>
#include <vector>
#include "IO/io.h"


class XmlAttributes
{
public:
	void clear() {_attributes.clear();}
	void add(const std::wstring &name, const std::wstring &value)
	  {_attributes.push_back(std::pair<std::wstring, std::wstring>(name, value));}
	size_t count() const {return _attributes.size();}
	std::wstring name(size_t index) const {return _attributes[index].first;}
	std::wstring value(size_t index) const {return _attributes[index].second;}

private:
	std::vector<std::pair<std::wstring, std::wstring> > _attributes;
};

class XmlHandler
{
public:
	virtual bool startDocument() = 0;
	virtual bool endDocument() = 0;
	virtual bool startElement(const std::wstring &name,
	  const XmlAttributes &atts) = 0;
	virtual bool endElement(const std::wstring &name) = 0;
	virtual bool data(const std::wstring &data) = 0;
};

class XmlParser : public IO
{
public:
	void setHandler(XmlHandler *handler);
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
		AMP,		/* '&' */
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
	XmlAttributes _attributes;
};

#endif // XML_H_
