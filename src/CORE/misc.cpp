#include "misc.h"

using namespace std;

wstring s2w(const string &s)
{
	wstring w(s.begin(), s.end());
	return w;
}

wstring trim(const wstring &str)
{
	size_t s = str.find_first_not_of(L" \n\r\t");
	size_t e = str.find_last_not_of (L" \n\r\t");

	if ((wstring::npos == s) || (wstring::npos == e))
		return L"";
	else
		return str.substr(s, e - s + 1);
}

void stringReplace(wstring &source, const wstring &find,
  const wstring &replace)
{
	size_t j = 0;

	while ((j = source.find(find, j)) != wstring::npos) {
		source.replace(j, find.length(), replace);
		j += replace.length();
	}
}

bool stringCaseCmp(const wstring &s1, const wstring &s2)
{
	if (s1.length() != s2.length())
		return false;

	for (size_t i = 0; i < s1.length(); i++)
		if (tolower(s1[i]) != tolower(s2[i]))
			return false;

	return true;
}
