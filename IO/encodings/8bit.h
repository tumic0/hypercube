#ifndef EIGHTBIT_H_
#define EIGHTBIT_H_

#include <cstddef>

struct cw {
	unsigned char c;
	wchar_t w;
};

int do_in(const char* from, const char* from_end, const char*& from_next,
  wchar_t* to, wchar_t* to_limit, wchar_t*& to_next, unsigned char offset,
  const wchar_t *table);
int do_out(const wchar_t* from, const wchar_t* from_end,
  const wchar_t*& from_next, char* to, char* to_limit, char*& to_next,
  unsigned char offset, const struct cw *table, size_t table_size);

#endif // EIGHTBIT_H_
