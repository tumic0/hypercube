#ifndef MATRIX_H_
#define MATRIX_H_

#include <fstream>
#include "IO/io.h"


class MatrixGraphInput: public InputProvider
{
public:
	virtual Error readGraph(Graph *graph, const char *fileName,
	  Encoding *encoding);

private:
	enum Token {
		START,		/* Initial value */
		EOI,		/* End of File */
		ERROR,		/* Parse error */
		NL,			/* New line */
		ID			/* ID */
	};

	void error();
	void nextToken();
	void compare(Token token);
	unsigned count();
	void matrix(unsigned cnt);
	bool parse();

	std::wifstream _fs;
	Token _token;
	unsigned _line;
	unsigned _id;
	Graph *_graph;
};

#endif /* MATRIX_H_ */
