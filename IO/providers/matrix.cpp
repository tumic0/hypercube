#include <cstring>
#include <cerrno>
#include <fstream>
#include <sstream>
#include "CORE/vertex.h"
#include "CORE/edge.h"
#include "matrix.h"

using namespace std;


void MatrixGraphInput::error()
{
	if (_token == ERROR)
		return;

	ioerr << "MATRIX: parse error on line: " << _line << endl;
	_token = ERROR;
}

void MatrixGraphInput::nextToken()
{
	int c, state = 0;

	while (1) {
		c = _fs.get();

		switch (state) {
			case 0:
				if (c == ' ' || c == '\t')
					break;
				if (c == '\r') {
					state = 1;
					break;
				}
				if (c == '\n') {
					_line++;
					_token = NL;
					return;
				}
				if (isdigit(c)) {
					_id = c - '0';
					state = 2;
					break;
				}
				if (c == EOF) {
					_token = EOI;
					return;
				}

				error();
				return;

			case 1:
				if (c == '\n') {
					_line++;
					_token = NL;
					return;
				}
				error();
				return;

			case 2:
				if (isdigit(c)) {
					_id = _id * 10 + c - '0';
					break;
				}
				_fs.unget();
				_token = ID;
				return;
		}
	}
}

void MatrixGraphInput::compare(Token token)
{
	if (_token == token)
		nextToken();
	else
		error();
}

unsigned MatrixGraphInput::count()
{
	unsigned cnt = _id;

	compare(ID);
	compare(NL);

	for (unsigned i = 0; i < cnt; i++) {
		wstringstream ss;
		ss << i;

		Vertex *v = _graph->addVertex();
		v->setText(ss.str());
	}

	return cnt;
}

void MatrixGraphInput::matrix(unsigned cnt)
{
	for (unsigned i = 0; i < cnt; i++) {
		for (unsigned j = 0; j < cnt; j++) {
			if (_id > 0) {
				wstringstream ss;
				ss << _id;

				Edge *e = _graph->addEdge(_graph->vertex(i), _graph->vertex(j));
				e->setText(ss.str());
			}

			compare(ID);
		}
		compare(NL);
	}
}

bool MatrixGraphInput::parse()
{
	_line = 1;
	_token = START;

	nextToken();
	matrix(count());

	if (_token == EOI)
		return true;
	else {
		error();
		return false;
	}
}


void MatrixGraphInput::setInputEncoding(Encoding *encoding)
{
	_encoding = encoding;
}

IO::Error MatrixGraphInput::readGraph(Graph *graph, const char *fileName)
{
	IO::Error err = Ok;

	_graph = graph;

	_fs.open(fileName);
	if (!_fs) {
		ioerr << fileName << ": " << strerror(errno) << endl;
		err = OpenError;
	} else {
		if (!parse())
			err = (_fs.fail()) ? ReadError : FormatError;
	}

	_fs.close();
	_fs.clear();

	if (err)
		graph->clear();

	return err;
}
