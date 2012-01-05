#include <cstdlib>
#include "latin1cvt.h"


using namespace std;

codecvt_base::result latin1cvt::do_in(mbstate_t&, const char* from,
  const char* from_end, const char*& from_next, wchar_t* to,
  wchar_t* to_limit, wchar_t*& to_next) const
{
	for (from_next = from, to_next = to; from_next < from_end
	  && to_next < to_limit; from_next++, to_next++)
			*to_next = (unsigned char)*from_next;

	return codecvt_base::ok;
}

codecvt_base::result latin1cvt::do_out(mbstate_t&, const wchar_t* from,
  const wchar_t* from_end, const wchar_t*& from_next, char* to,
  char* to_limit, char*& to_next) const
{
	for (from_next = from, to_next = to; from_next < from_end
	  && to_next < to_limit; from_next++, to_next++) {
		if (*from_next <= 0xFF)
			*to_next = (char)*from_next;
		else
			return codecvt_base::error;
	}

	return codecvt_base::ok;
}

codecvt_base::result latin1cvt::do_unshift(mbstate_t&, char* to, char*,
  char*& to_next) const
{
	to_next = to;

	return codecvt_base::noconv;
}
