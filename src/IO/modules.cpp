#include "providers/matrix.h"
#include "providers/list.h"
#include "providers/graphml.h"
#include "providers/gxl.h"
#include "providers/dot.h"
#include "providers/gml.h"
#include "providers/svg.h"
#include "providers/ps.h"
#include "encodings/utf8.h"
#include "encodings/iso1.h"
#include "encodings/iso2.h"
#include "encodings/iso5.h"
#include "encodings/iso7.h"
#include "encodings/cp1250.h"
#include "encodings/cp1251.h"
#include "encodings/cp1252.h"
#include "encodings/cp1253.h"
#include "encodings/koi8r.h"
#include "encodings/koi8u.h"
#include "modules.h"


OutputProvider* outputProviders[] = {
	new SvgGraphOutput,
	new PsGraphOutput,
	0
};

InputProvider* inputProviders[] = {
	new GxlGraphInput,
	new GraphmlGraphInput,
	new DotGraphInput,
	new GmlGraphInput,
	new ListGraphInput,
	new MatrixGraphInput,
	0
};

Encoding* encodings[] = {
	new ISO1,
	new ISO2,
	new ISO5,
	new ISO7,
	new CP1250,
	new CP1251,
	new CP1252,
	new CP1253,
	new KOI8R,
	new KOI8U,
	new UTF8,
	0
};
