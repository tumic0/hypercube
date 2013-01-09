#ifndef DOT_H_
#define DOT_H_

#include <string>
#include <fstream>
#include <map>
#include <set>
#include "IO/io.h"

class DotGraphInput: public InputProvider
{
public:
	virtual Error readGraph(Graph *graph, const char *fileName,
	  Encoding *encoding);

private:
	enum Token {
		START,		/* Initial value */
		EOI,		/* End of File */
		ERROR,		/* Parse error */

		COLON,		/* ':' */
		SEMICOLON,	/* ';' */
		COMMA,		/* ',' */
		EQ,			/* '=' */
		LBRC,		/* '{' */
		RBRC,		/* '}' */
		LBRK,		/* '[' */
		RBRK,		/* ']' */
		EDGEOP,		/* Edge operator ('--' or '->') */

		ID,			/* ID (string | quoted string | number) */
		NODE,		/* 'Node' keyword */
		EDGE,		/* 'Edge' keyword */
		GRAPH,		/* 'Graph' keyword */
		DIGRAPH,	/* 'Digraph' keyword */
		SUBGRAPH,	/* 'Subgraph' keyword */
		STRICT		/* 'Strict' keyword */
	};

	struct Keyword {
		Token token;
		std::wstring name;
	};

	struct Attributes {
		std::wstring label;
	};

	typedef std::set<std::wstring> idSet;
	typedef std::set<Edge*> edgeSet;


	Token keyword();
	void error();
	void nextToken();
	void compare(Token token);
	void list(Attributes &attr);
	void attributeList(Attributes &attr);
	void edge(idSet &src, edgeSet &edges);
	void subgraph(idSet &parent);
	void subgraphStatement(idSet &vertexes);
	void compassPt();
	void nodeId();
	void idStatement(idSet &vertexes);
	void attributeStatement();
	void statementList(idSet &vertexes);
	void graphId();
	void graphType();
	void graph();
	bool parse();

	Edge* addEdge(const std::wstring &src, const std::wstring &dst);
	Vertex* addVertex(const std::wstring &vertex);

	void setVertexAttributes(Vertex *vertex, const Attributes &attr);
	void setEdgeAttributes(Edge *edge, const Attributes &attr);
	void mergeAttributes(Attributes &dst, const Attributes &src);
	void setAttribute(Attributes &attr, const std::wstring &key,
	  const std::wstring &value);
	void attributesClear();

	static const Keyword keywords[];

	std::wstring _id;
	lexstream _fs;
	Token _token;
	unsigned _line;

	std::map<std::wstring, Vertex*> _vertexes;
	Attributes _nodeAttributes, _edgeAttributes;

	Graph *_graph;
};

#endif // DOT_H_
