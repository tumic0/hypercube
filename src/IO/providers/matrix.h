#ifndef MATRIX_H_
#define MATRIX_H_

#include "IO/io.h"
#include "IO/lexstream.h"


class MatrixGraphInput: public InputProvider
{
public:
	MatrixGraphInput() : _encoding(0) {}
	virtual Error readGraph(Graph *graph, const char *fileName);
	virtual void setInputEncoding(Encoding *encoding);

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

	lexstream _fs;
	Token _token;
	unsigned _line;
	unsigned _id;

	Graph *_graph;
	Encoding *_encoding;
};

#endif /* MATRIX_H_ */
