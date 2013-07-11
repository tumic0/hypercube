#include <cstring>
#include <cerrno>
#include "CORE/misc.h"
#include "IO/modules.h"
#include "IO/encodings/utf8cvt.h"
#include "IO/encodings/iso1.h"
#include "xml.h"

using namespace std;


#define isStartName(c) \
	(isalnum((c)) || (c) == '_' || (c) == ':')
#define isName(c) \
	(isStartName((c)) || (c) == '-' || (c) == '.')


const unsigned char BOM[] = {0xEF, 0xBB, 0xBF};

static void escape(wstring &str)
{
	stringReplace(str, L"&amp;", L"&");
	stringReplace(str, L"&lt;", L"<");
	stringReplace(str, L"&gt;", L">");
	stringReplace(str, L"&quot;", L"\"");
	stringReplace(str, L"&apos;", L"\'");
}

void XmlParser::setEncoding(const wstring &encoding)
{
	codecvt<wchar_t,char,mbstate_t> *cvt = 0;

	for (Encoding **ep = encodings; *ep; ep++) {
		if (stringCaseCmp(encoding, s2w((*ep)->name()))) {
			cvt = (*ep)->cvt();
			break;
		}
	}
	if (!cvt) {
		cerr << "Unsupported encoding. Using UTF-8." << endl;
		cvt = new utf8cvt;
	}

	locale lc(std::locale(), cvt);
	_fs.imbue(lc);
}

void XmlParser::error()
{
	if (_token == ERROR)
		return;

	ioerr << _errorPrefix << ": parse error on line: " << _line << endl;
	_token = ERROR;
}

void XmlParser::nextToken()
{
	int c, state = 0;


	while (1) {
		c = _fs.get();

		switch (state) {
			case 0:
				if (isspace(c)) {
					if (c == '\n')
						_line++;
					break;
				}
				if (c == '<') {
					_token = LT;
					return;
				}
				if (c == '>') {
					_token = GT;
					return;
				}
				if (c == '[') {
					_token = LSB;
					return;
				}
				if (c == ']') {
					_token = RSB;
					return;
				}
				if (c == '=') {
					_token = EQ;
					return;
				}
				if (c == '/') {
					_token = SLASH;
					return;
				}
				if (c == '?') {
					_token = QM;
					return;
				}
				if (c == '!') {
					_token = EXCL;
					return;
				}
				if (c == '-') {
					_token = MINUS;
					return;
				}
				if (isStartName(c)) {
					_string = c;
					state = 1;
					break;
				}
				if (c == '"') {
					_string.clear();
					state = 2;
					break;
				}
				if (c == '\'') {
					_string.clear();
					state = 3;
					break;
				}
				if (c == EOF) {
					_token = EOI;
					return;
				}
				_token = DATA;
				return;

			case 1:
				if (isName(c)) {
					_string += c;
					break;
				}
				_fs.unget();
				_token = IDENT;
				return;

			case 2:
				if (c == EOF) {
					error();
					return;
				}
				if (c == '"') {
					_token = STRING;
					return;
				}
				if (c == '\n')
					_line++;
				_string += c;
				break;

			case 3:
				if (c == EOF) {
					error();
					return;
				}
				if (c == '\'') {
					_token = STRING;
					return;
				}
				if (c == '\n')
					_line++;
				_string += c;
				break;
		}
	}
}

void XmlParser::compare(Token token)
{
	if (_token == token)
		nextToken();
	else
		error();
}

void XmlParser::data()
{
	int c;

	_string.clear();

	while (1) {
		c = _fs.get();

		if (c == '<') {
			escape(_string);
			_handler->data(_string);
			_token = LT;
			return;
		} else if (c == EOF) {
			_token = EOI;
			return;
		} else if (c == '\n')
			_line++;

		_string += c;
	}
}

void XmlParser::dtdElement()
{
	while (1) {
		switch (_token) {
			case GT:
				nextToken();
				return;
			case ERROR:
				return;
			case EOI:
				error();
				return;
			default:
				nextToken();
		}
	}
}

void XmlParser::dtdElementType()
{
	switch (_token) {
		case MINUS:
			comment();
			break;
		case IDENT:
			dtdElement();
			break;
		default:
			error();
	}
}

void XmlParser::dtdData()
{
	while (1) {
		switch (_token) {
			case LT:
				nextToken();
				compare(EXCL);
				dtdElementType();
				break;
			case RSB:
				nextToken();
				return;
			default:
				error();
				return;
		}
	}
}

void XmlParser::cdataData()
{
	int c, state = 0;

	_string.clear();

	while (1) {
		c = _fs.get();

		_string += c;
		if (c == '\n')
			_line++;

		switch (state) {
			case 0:
				if (c == EOF) {
					error();
					return;
				}
				if (c == ']')
					state = 1;
				break;
			case 1:
				if (c == EOF) {
					error();
					return;
				}
				if (c == ']')
					state = 2;
				else
					state = 0;
				break;
			case 2:
				if (c == '>') {
					_string.resize(_string.length() - 3);
					_handler->data(_string);
					_token = GT;
					return;
				}
				error();
				return;
		}
	}
}

void XmlParser::commentData()
{
	int c, state = 0;

	while (1) {
		c = _fs.get();

		if (c == '\n')
			_line++;

		switch (state) {
			case 0:
				if (c == EOF) {
					error();
					return;
				}
				if (c == '-')
					state = 1;
				break;
			case 1:
				if (c == EOF) {
					error();
					return;
				}
				if (c == '-')
					state = 2;
				else
					state = 0;
				break;
			case 2:
				if (c == '>') {
					_token = GT;
					return;
				}
				error();
				return;
		}
	}
}

void XmlParser::comment()
{
	compare(MINUS);
	if (_token != MINUS) {
		error();
		return;
	}
	commentData();
	compare(GT);
}

void XmlParser::cdata()
{
	compare(LSB);
	compare(IDENT);
	if (_string != L"CDATA")
		error();
	if (_token != LSB) {
		error();
		return;
	}
	cdataData();
	compare(GT);
}

void XmlParser::special()
{
	switch (_token) {
		case MINUS:
			comment();
			break;
		case LSB:
			cdata();
			break;
		default:
			error();
	}
}

void XmlParser::attribute(bool xml)
{
	wstring attr, value;

	attr = _string;
	compare(IDENT);
	compare(EQ);
	value = _string;
	compare(STRING);

	if (_token == ERROR)
		return;
	if (xml) {
		if (attr == L"encoding")
			_encoding = value;
	} else {
		escape(value);
		_handler->attribute(attr, value);
	}
}

void XmlParser::xmlAttributes()
{
	while (1) {
		switch (_token) {
			case QM:
			case ERROR:
				return;
			default:
				attribute(true);
		}
	}
}

bool XmlParser::attributes(bool xml)
{
	while (1) {
		switch (_token) {
			case SLASH:
				nextToken();
				return true;
			case GT:
				return false;
			case IDENT:
				attribute(xml);
				break;
			default:
				error();
				return false;
		}
	}
}

void XmlParser::elementType()
{
	switch (_token) {
		case SLASH:
			break;
		case IDENT:
			element();
			break;
		case EXCL:
			nextToken();
			special();
			break;
		default:
			error();
	}
}

void XmlParser::nextItem()
{
	switch (_token) {
		case LT:
			nextToken();
			elementType();
			break;
		default:
			error();
	}
}

void XmlParser::elementEnd()
{
	if (_token != GT) {
		error();
		return;
	}
	data();
}

void XmlParser::element()
{
	bool closed;
	wstring start, end;


	start = _string;
	compare(IDENT);
	if (_token != ERROR)
		if (!_handler->startElement(start))
			error();
	closed = attributes(false);
	elementEnd();

	if (closed) {
		if (_token != ERROR)
			if (!_handler->endElement(start))
				error();
		return;
	}

	do {
		nextItem();
	} while (_token != ERROR && _token != SLASH);

	compare(SLASH);
	end = _string;
	compare(IDENT);
	elementEnd();

	if (start != end)
		error();

	if (_token != ERROR)
		if (!_handler->endElement(end))
			error();
}

void XmlParser::dtdMarkupDecl()
{
	switch (_token) {
		case LSB:
			nextToken();
			dtdData();
			break;
		case GT:
			return;
		default:
			error();
	}
}

void XmlParser::dtdExternalId()
{
	wstring type = _string;

	switch (_token) {
		case LSB:
			break;
		case IDENT:
			nextToken();
			if (type == L"PUBLIC") {
				compare(STRING);
				compare(STRING);
			} else if (type == L"SYSTEM")
				compare(STRING);
			else
				error();
			break;
		default:
			error();
	}
}

void XmlParser::dtdDecl()
{
	if (_string != L"DOCTYPE")
		error();
	compare(IDENT);
	compare(IDENT);
	dtdExternalId();
	dtdMarkupDecl();
	compare(GT);
}

void XmlParser::xmlDecl()
{
	compare(QM);
	if (!stringCaseCmp(_string, L"xml"))
		error();
	compare(IDENT);
	xmlAttributes();
	compare(QM);
	compare(GT);
}

void XmlParser::prologComment()
{
	switch (_token) {
		case EXCL:
			nextToken();
			comment();
			compare(LT);
			prologComment();
			break;
		case IDENT:
			break;
		default:
			error();
	}
}

void XmlParser::prologSpecial()
{
	switch (_token) {
		case MINUS:
			comment();
			compare(LT);
			prologContent();
			break;
		case IDENT:
			dtdDecl();
			compare(LT);
			prologComment();
			break;
		default:
			error();
	}
}

void XmlParser::prologContent()
{
	switch (_token) {
		case EXCL:
			nextToken();
			prologSpecial();
			break;
		case IDENT:
			break;
		default:
			error();
	}
}

void XmlParser::xml()
{
	compare(LT);

	switch (_token) {
		case QM:
			xmlDecl();
			compare(LT);
			prologContent();
			break;
		case EXCL:
			nextToken();
			prologSpecial();
			break;
		case IDENT:
			break;
		default:
			error();
			return;
	}

	setEncoding(_encoding);
	element();
}

void XmlParser::bom()
{
	unsigned char bom[sizeof(BOM)];

	if (_fs.peek() == BOM[0]) {
		for (size_t i = 0; i < sizeof(BOM); i++)
			bom[i] = _fs.get();
		if (memcmp(bom, BOM, sizeof(BOM))) {
			error();
			return;
		}
	}

	nextToken();
}

bool XmlParser::start()
{
	_line = 1;
	_token = START;
	_encoding = L"utf-8";

	_handler->startDocument();
	bom();
	xml();
	_handler->endDocument();

	if (_token == EOI)
		return true;
	else {
		error();
		return false;
	}
}


void XmlParser::setErrorPrefix(const std::string &prefix)
{
	_errorPrefix = prefix;
}

IO::Error XmlParser::parse(const char *fileName)
{
	IO::Error err = Ok;

	locale lc(std::locale(), new iso1cvt);
	_fs.imbue(lc);

	_fs.open(fileName);
	if (!_fs) {
		ioerr << fileName << ": " << strerror(errno) << endl;
		err = OpenError;
	} else {
		if (!start())
			err = (_fs.fail()) ? ReadError : FormatError;
	}

	_fs.close();
	_fs.clear();

	return err;
}
