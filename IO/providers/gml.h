#ifndef GML_H_
#define GML_H_

#include <string>
#include <fstream>
#include "IO/io.h"


class GmlGraphInput : public InputProvider
{
public:
	virtual Error readGraph(Graph *graph, const char *fileName,
	  Encoding *encoding);

private:
	enum Token {
		START,		/* Initial value */
		EOI,		/* End of File */
		ERROR,		/* Parse error */

		INT,		/* Integer */
		REAL,		/* Real */
		STRING,		/* String */
		KEY,		/* Key (identificator) */

		LBRK,		/* '[' */
		RBRK		/* ']' */
	};

	enum ValueType {
		UNKNOWN,
		GRAPH,
		NODE,
		EDGE,
		ID,
		LABEL,
		SOURCE,
		TARGET
	};

	struct NodeAttributes {
		int id;
		std::wstring label;
	};

	struct EdgeAttributes {
		int source;
		int target;
		std::wstring label;
	};

	void error();
	void nextToken();
	void compare(Token token);
	void value(ValueType parent, ValueType key);
	void list(ValueType parent);
	bool parse();

	ValueType valueType();

	void attributesClear();
	void setVertexAttributes(Vertex *vertex);
	void setEdgeAttributes(Edge *edge);

	Vertex *addVertex(int id);
	Edge *addEdge();

	std::wifstream _fs;
	std::wstring _string;
	int _int;
	float _float;
	Token _token;
	unsigned _line;

	NodeAttributes _nodeAttributes;
	EdgeAttributes _edgeAttributes;

	std::map<int, Vertex*> _vertexes;

	Graph *_graph;
};

#endif // GML_H_