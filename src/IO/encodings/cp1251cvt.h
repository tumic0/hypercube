#ifndef CP1251CVT_H_
#define CP1251CVT_H_

#include <locale>

class cp1251cvt : public std::codecvt<wchar_t,char,mbstate_t>
{
protected:
	virtual bool do_always_noconv() const throw() { return false; }
	virtual int do_max_length() const throw() { return 1; }
	virtual int do_encoding() const throw() { return 1; }
	virtual int do_length(mbstate_t&, const char*, const char*,
	  size_t max) const { return max; }
	virtual result do_in(mbstate_t& state, const char* from,
	  const char* from_end, const char*& from_next, wchar_t* to,
	  wchar_t* to_limit, wchar_t*& to_next) const;
	virtual result do_out(mbstate_t& state, const wchar_t* from,
	  const wchar_t* from_end, const wchar_t*& from_next, char* to,
	  char* to_limit, char*& to_next) const;
	virtual result do_unshift(mbstate_t& state, char* to, char* to_limit,
	  char*& to_next) const;
};

#endif /* CP1251CVT_H_ */
