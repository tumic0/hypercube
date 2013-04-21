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

private:
	enum Token {
		START,		/* Initial value */
		EOI,		/* End of File */
		ERROR,		/* Parse error */

		STRING,		/* Quoted string */
		IDENT,		/* Identificator (unknown) */
		DATA,		/* Character data */

		LT,			/* '<' */
		GT,			/* '>' */
		LSB,		/* '[' */
		RSB,		/* ']' */
		EQ,			/* '=' */
		AMP,		/* '&' */
		SLASH,		/* '/' */
		QM,			/* '?' */
		EXCL,		/* '!' */
		MINUS		/* '-' */
	};

	struct Relation {
		std::wstring node;
		std::wstring parent;
	};

	struct GraphAttributes {
		std::wstring encoding;
		std::wstring edgedefault;
	};

	struct NodeAttributes {
		std::wstring id;
	};

	struct EdgeAttributes {
		std::wstring id;
		std::wstring source;
		std::wstring target;
	};


	Vertex *addVertex(const std::wstring &id);
	Edge *addEdge(const std::wstring &source, const std::wstring &target);

	void initGraphAttributes();
	void clearNodeAttributes();
	void clearEdgeAttributes();
	void setAttribute(const std::wstring &element, const std::wstring &attr,
	  const std::wstring &value);
	void handleElement(const std::wstring &element);

	void setEncoding(const std::wstring &encoding);
	void checkRelation(const std::wstring &node, const std::wstring &parent);

	void error();
	void nextToken();
	void compare(Token token);
	void data();
	void dtdData();
	void cdataData();
	void commentData();
	void comment();
	void cdata();
	void special();
	void attribute(const std::wstring &element);
	bool attributes(const std::wstring &element);
	void nextItem(const std::wstring &parent);
	void elementType(const std::wstring &parent);
	void element(const std::wstring &parent);
	void elementEnd();
	void prologSpecial();
	void prologContent();
	void prologComment();
	void dtdElement();
	void dtdElementType();
	void dtdMarkupDecl();
	void dtdExternalId();
	void dtdDecl();
	void xmlAttributes();
	void xmlDecl();
	void xml();
	void bom();
	bool parse();

	std::wstring _string;
	lexstream _fs;
	Token _token;
	unsigned _line;
	static const Relation relations[];

	GraphAttributes _graphAttributes;
	NodeAttributes _nodeAttributes;
	EdgeAttributes _edgeAttributes;
	std::map<std::wstring, Vertex*> _vertexes;
	Graph *_graph;
};

#endif // GRAPHML_H_
