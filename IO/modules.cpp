#include <cstdlib>
#include "providers/matrix.h"
#include "providers/list.h"
#include "providers/svg.h"
#include "providers/ps.h"
#include "encodings/utf8.h"
#include "encodings/latin1.h"
#include "encodings/latin2.h"
#include "encodings/koi8r.h"
#include "encodings/cp1250.h"


OutputProvider* outputProviders[] = {
	new SvgGraphOutput,
	new PsGraphOutput,
	NULL
};

InputProvider* inputProviders[] = {
	new ListGraphInput,
	new MatrixGraphInput,
	NULL
};

Encoding* encodings[] = {
	new Latin1,
	new Latin2,
	new KOI8R,
	new CP1250,
	new UTF8,
	NULL
};
