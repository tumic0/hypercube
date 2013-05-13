#include <cstdlib>
#include "8bit.h"


static int cmp(const void *p1, const void *p2)
{
	struct cw *cw1 = (struct cw*) p1;
	struct cw *cw2 = (struct cw*) p2;

	return (int)cw1->w - (int)cw2->w;
}


int do_in(const char* from, const char* from_end, const char*& from_next,
  wchar_t* to, wchar_t* to_limit, wchar_t*& to_next, unsigned char offset,
  const wchar_t *table)
{
	for (from_next = from, to_next = to; from_next < from_end
	  && to_next < to_limit; from_next++, to_next++) {
		if ((unsigned char)*from_next < offset)
			*to_next = (unsigned char)*from_next;
		else {
			*to_next = table[(unsigned char)*from_next - offset];
			if (!*to_next)
				return -1;
		}
	}

	return 0;
}

int do_out(const wchar_t* from, const wchar_t* from_end,
  const wchar_t*& from_next, char* to, char* to_limit, char*& to_next,
  unsigned char offset, const struct cw *table, size_t table_size)
{
	cw key, *res;

	for (from_next = from, to_next = to; from_next < from_end
	  && to_next < to_limit; from_next++, to_next++) {
		if (*from_next < offset)
			*to_next = (char)*from_next;
		else {
			key.w = *from_next;
			if ((res = (struct cw*) bsearch(&key, table,
			  table_size / sizeof(struct cw),
			  sizeof(struct cw), cmp)) == NULL)
				return -1;

			*to_next = res->c;
		}
	}

	return 0;
}
