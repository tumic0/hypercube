#include "providers/matrix.h"
#include "providers/list.h"
#include "providers/dot.h"
#include "providers/gml.h"
#include "providers/svg.h"
#include "providers/ps.h"
#include "encodings/utf8.h"
#include "encodings/latin1.h"
#include "encodings/latin2.h"
#include "encodings/koi8r.h"
#include "encodings/cp1250.h"
#include "modules.h"


OutputProvider* outputProviders[] = {
	new SvgGraphOutput,
	new PsGraphOutput,
	0
};

InputProvider* inputProviders[] = {
	new DotGraphInput,
	new GmlGraphInput,
	new ListGraphInput,
	new MatrixGraphInput,
	0
};

Encoding* encodings[] = {
	new Latin1,
	new Latin2,
	new KOI8R,
	new CP1250,
	new UTF8,
	0
};
