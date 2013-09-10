/*
	This source file is based on Arabica (http://www.jezuk.co.uk/arabica)
	code with the following licence:


	Copyright 2002-2008 Jez UK Ltd
	All rights reserved

	Redistribution and use in source and binary forms, with or
	without modification, are permitted provided that the following
	conditions are met:

	* Redistributions of source code must retain the above
	  copyright notice, this list of conditions and the following
	  disclaimer.
	* Redistributions in binary form must reproduce the above
	  copyright notice, this list of conditions and the following
	  disclaimer in the documentation and/or other materials
	  provided with the distribution.
	* Neither the name of Jez UK Ltd nor the names of
	  contributors may be used to endorse or promote products
	  derived from this software without specific prior written
	  permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
	CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
	NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
	LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
	ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/

#include "utf8cvt.h"

using namespace std;


struct Tab {
	unsigned char char_mask;
	unsigned char char_value;
	int shift;
	wchar_t wide_mask;
};

static const Tab tab[] = {
	{0x80,  0x00,   0*6,    0x7F,   },
	{0xE0,  0xC0,   1*6,    0x7FF,  },
	{0xF0,  0xE0,   2*6,    0xFFFF, },
	{0,     0,      0,      0,      }
};


int utf8cvt::do_length(mbstate_t&, const char* from,
  const char* end, size_t max) const
{
	size_t count = 0;
	const char* next = from;

	while ((next < end) && (count < max)) {
		if (!(*next & 0x80)) {
			++count;
			++next;
		} else if ((*next & 0xC0) == 0xC0) {
			if (next + 2 < end) {
				++count;
				next += 2;
			} else
				break;
		} else if ((*next & 0xE0) == 0xE0) {
			if (next + 3 < end) {
				++count;
				next += 3;
			} else
				break;
	    }
	}

	return (next - from);
}

codecvt_base::result utf8cvt::do_in(mbstate_t&, const char* from,
  const char* from_end, const char*& from_next, wchar_t* to,
  wchar_t* to_limit, wchar_t*& to_next) const
{
	from_next = from;
	to_next = to;

	while ((from_next < from_end) && (to_next < to_limit)) {
		unsigned char start = static_cast<unsigned char>(*from_next);

		const Tab *t = tab;
		for (; t->char_mask; ++t) {
			if ((start & t->char_mask) == t->char_value)
				break;
		}

		if ((from_next + (t - tab)) >= from_end)
			break;

		wchar_t wide_mask = t->wide_mask;

		*to_next = start;
		for (; t != tab; --t) {
			from_next++;
			*to_next = (*to_next << 6) | ((*from_next ^ 0x80) & 0xFF);
		}
		*to_next &= wide_mask;

		++from_next;
		++to_next;
	}

	return (from_next == from_end)
	  ? std::codecvt_base::ok : std::codecvt_base::partial;
}

codecvt_base::result utf8cvt::do_out(mbstate_t&, const wchar_t* from,
  const wchar_t* from_end, const wchar_t*& from_next, char* to,
  char* to_limit, char*& to_next) const
{
	from_next = from;
	to_next = to;

	while (from_next < from_end) {
		wchar_t fn = static_cast<wchar_t>(*from_next);

		for (const Tab *t = tab; t->char_mask; t++) {
			if (fn > t->wide_mask )
				continue;

			if (to_next + (t - tab) + 1 >= to_limit)
				return std::codecvt_base::partial;

			int c = t->shift;
			*to_next++ = static_cast<char>(t->char_value | (fn >> c));
			while (c > 0) {
				c -= 6;
				*to_next++ = static_cast<char>(0x80 | ((fn >> c) & 0x3F));
			}
			break;
		}
	    ++from_next;
	}

	return std::codecvt_base::ok;
}

codecvt_base::result utf8cvt::do_unshift(mbstate_t&, char* to, char*,
  char*& to_next) const
{
	to_next = to;

	return codecvt_base::noconv;
}
