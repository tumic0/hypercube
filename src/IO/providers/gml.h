#ifndef GML_H_
#define GML_H_

#include <string>
#include "IO/io.h"


class GmlGraphInput : public InputProvider
{
public:
	GmlGraphInput() : _encoding(0) {}
	virtual Error readGraph(Graph *graph, const char *fileName);
	virtual void setInputEncoding(Encoding *encoding);
	virtual void setNodeLabelAttribute(const char *name);
	virtual void setEdgeLabelAttribute(const char *name);

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

	struct Relation {
		const std::wstring node;
		const std::wstring parent;
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

	struct GraphAttributes {
		bool directed;
	};

	void error();
	void nextToken();
	void compare(Token token);
	void value(const std::wstring & parent, const std::wstring &key);
	void list(const std::wstring &parent);
	bool parse();

	bool checkRelation(const std::wstring &key, const std::wstring &parent);
	bool handleKey(const std::wstring &key);

	void clearNodeAttributes();
	void clearEdgeAttributes();
	void initGraphAttributes();
	void setIntAttribute(const std::wstring &parent, const std::wstring &key,
	  int value);
	void setStringAttribute(const std::wstring &parent, const std::wstring &key,
	  const std::wstring &value);
	void setVertexAttributes(Vertex *vertex);
	void setEdgeAttributes(Edge *edge);

	Vertex *addVertex(int id);
	Edge *addEdge(int source, int target);

	std::wstring _string;
	lexstream _fs;
	int _int;
	float _float;
	Token _token;
	unsigned _line;

	GraphAttributes _graphAttributes;
	NodeAttributes _nodeAttributes;
	EdgeAttributes _edgeAttributes;
	std::wstring _nodeLabelAttr, _edgeLabelAttr;

	std::map<int, Vertex*> _vertexes;

	Graph *_graph;
	Encoding *_encoding;

	static const Relation relations[];
};

#endif // GML_H_