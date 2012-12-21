#ifndef GRAPHML_H_
#define GRAPHML_H_

#include <string>
#include <fstream>
#include "IO/io.h"


class GraphmlGraphInput : public InputProvider
{
public:
	virtual Error readGraph(Graph *graph, const char *fileName,
	  Encoding *encoding);

public:
	enum Token {
		START,		/* Initial value */
		EOI,		/* End of File */
		ERROR,		/* Parse error */

		STRING,		/* Quoted string */
		IDENT,		/* Identificator (unknown) */

		LT,			/* '<' */
		GT,			/* '>' */
		EQ,			/* '=' */
		AMP,		/* '&' */
		SLASH,		/* '/' */
		QM,			/* '?' */
	};

	struct Relation {
		std::wstring node;
		std::wstring parent;
	};

	struct Attributes {
		std::wstring id;
		std::wstring source;
		std::wstring target;
	};

	Vertex *addVertex(const std::wstring &id);
	Edge *addEdge(const std::wstring &source, const std::wstring &target);

	void checkRelation(const std::wstring &node, const std::wstring &parent);

	void error();
	void nextToken();
	void compare(Token token);
	void data();
	void attribute();
	bool attributes();
	void nextItem(const std::wstring &parent);
	void elementType(const std::wstring &parent);
	void element(const std::wstring &parent);
	void xmlAttributes();
	void xmlProlog();
	void xml();
	bool parse();

	std::wifstream _fs;
	std::wstring _string;
	Token _token;
	unsigned _line;
	static const Relation relations[];

	Attributes _attributes;
	std::map<std::wstring, Vertex*> _vertexes;
	Graph *_graph;
};

#endif // GRAPHML_H_
