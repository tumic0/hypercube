#include <cstring>
#include "io.h"
#include "matrix.h"
#include "svg.h"
#include "ps.h"


/*!
	\class IO
	\brief Base class of all I/O related classes.

	Lorem ipsum...
*/

/*!
	\class OutputProvider
	\brief Defines the graph output provider interface.

	Lorem ipsum...
*/

/*!
	\class InputProvider
	\brief Defines the graph input provider interface.

	Lorem ipsum...
*/


OutputProvider* outputProviders[] = {
	new SvgGraphOutput(),
	new PsGraphOutput(),
	NULL
};

InputProvider* inputProviders[] = {
	new MatrixGraphInput(),
	NULL
};
