#ifndef MISC_H_
#define MISC_H_

#include <string>

#define MAX(a,b) \
	(((a) > (b)) ? (a) : (b))
#define MIN(a,b) \
	(((a) < (b)) ? (a) : (b))

#define ARRAY_SIZE(array) \
	(sizeof(array) / sizeof((array)[0]))

std::wstring trim(const std::wstring &str);
bool stringCaseCmp(const std::wstring &s1, const std::wstring &s2);
void stringReplace(std::wstring &source, const std::wstring &find,
  const std::wstring &replace);
std::wstring s2w(const std::string &s);

#endif /* MISC_H_ */
