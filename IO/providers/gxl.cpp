#include <string>
#include <map>
#include <stack>
#include "CORE/misc.h"
#include "IO/providers/xml/xml.h"
#include "gxl.h"

using namespace std;


class GxlHandler : public XmlHandler
{
public:
	GxlHandler(Graph *graph) : _graph(graph) {}

	virtual bool startDocument();
	virtual bool endDocument();
	virtual bool startElement(const std::wstring &name, const XmlAttributes &atts);
	virtual bool endElement(const wstring &name);
	virtual bool data(const wstring &data);

private:
	struct Relation {
		wstring node;
		wstring parent;
	};

	struct GraphAttributes {
		wstring edgemode;
	};

	struct NodeAttributes {
		wstring id;
	};

	struct EdgeAttributes {
		wstring id;
		wstring from;
		wstring to;
	};

	Vertex *addVertex(const wstring &id);
	Edge *addEdge(const wstring &source, const wstring &target);
	bool checkRelation(const wstring &node, const wstring &parent);
	void initGraphAttributes();
	void clearNodeAttributes();
	void clearEdgeAttributes();
	void setAttribute(const wstring &element, const wstring &attr,
	  const wstring &value);
	bool handleElement(const wstring &element);

	Graph *_graph;
	map<wstring, Vertex*> _vertexes;
	stack<wstring> _parent;
	GraphAttributes _graphAttributes;
	NodeAttributes _nodeAttributes;
	EdgeAttributes _edgeAttributes;

	static const Relation relations[];
};


const GxlHandler::Relation GxlHandler::relations[] = {
	{L"gxl", L""},
	{L"graph", L"gxl"},
	{L"node", L"graph"},
	{L"edge", L"graph"}
};


bool GxlHandler::startDocument()
{
	initGraphAttributes();

	return true;
}

bool GxlHandler::endDocument()
{
	bool directed = (_graphAttributes.edgemode == L"directed"
	  || _graphAttributes.edgemode == L"defaultdirected") ? true : false;
	_graph->setDirected(directed);

	return true;
}

bool GxlHandler::startElement(const std::wstring &name, const XmlAttributes &atts)
{
	if (!checkRelation(name, _parent.empty() ? L"" : _parent.top()))
		return false;

	for (size_t i = 0; i < atts.count(); i++)
		setAttribute(name, atts.name(i), atts.value(i));

	if (!handleElement(name))
		return false;

	_parent.push(name);

	return true;
}

bool GxlHandler::endElement(const wstring &)
{
	_parent.pop();

	return true;
}

bool GxlHandler::data(const wstring &)
{
	return true;
}


Vertex* GxlHandler::addVertex(const wstring &id)
{
	Vertex *v;
	map<wstring, Vertex*>::const_iterator it;

	it = _vertexes.find(id);
	if (it != _vertexes.end())
		return it->second;

	v = _graph->addVertex();

	_vertexes.insert(pair<wstring, Vertex*>(id, v));

	return v;
}

Edge* GxlHandler::addEdge(const wstring &source, const wstring &target)
{
	Vertex *src, *dst;

	src = addVertex(source);
	dst = addVertex(target);

	return _graph->addEdge(src, dst);
}

bool GxlHandler::checkRelation(const wstring &node, const wstring &parent)
{
	if (parent.empty()) {
		if (node != L"gxl")
			return false;
	}

	for (size_t i = 0; i < ARRAY_SIZE(relations); i++) {
		if (node == relations[i].node) {
			if (parent != relations[i].parent)
				return false;
		}
	}

	return true;
}

void GxlHandler::setAttribute(const wstring &element, const wstring &attr,
  const wstring &value)
{
	if (element == L"node") {
		if (attr == L"id")
			_nodeAttributes.id = value;
	} else if (element == L"edge") {
		if (attr == L"id")
			_edgeAttributes.id = value;
		if (attr == L"from")
			_edgeAttributes.from = value;
		if (attr == L"to")
			_edgeAttributes.to = value;
	} else if (element == L"graph") {
		if (attr == L"edgemode")
			_graphAttributes.edgemode = value;
	}
}

void GxlHandler::initGraphAttributes()
{
	_graphAttributes.edgemode = L"directed";
}

void GxlHandler::clearNodeAttributes()
{
	_nodeAttributes.id.clear();
}

void GxlHandler::clearEdgeAttributes()
{
	_edgeAttributes.id.clear();
	_edgeAttributes.from.clear();
	_edgeAttributes.to.clear();
}

bool GxlHandler::handleElement(const wstring &element)
{
	Vertex *vertex;
	Edge *edge;

	if (element == L"node") {
		if (_nodeAttributes.id.empty())
			return false;
		vertex = addVertex(_nodeAttributes.id);
		vertex->setText(_nodeAttributes.id);

		clearNodeAttributes();
	}
	if (element == L"edge") {
		if (_edgeAttributes.from.empty() || _edgeAttributes.to.empty())
			return false;
		edge = addEdge(_edgeAttributes.from, _edgeAttributes.to);
		if (!_edgeAttributes.id.empty())
			edge->setText(_edgeAttributes.id);

		clearEdgeAttributes();
	}

	return true;
}


IO::Error GxlGraphInput::readGraph(Graph *graph, const char *fileName,
  Encoding *)
{
	XmlParser parser;
	GxlHandler handler(graph);

	parser.setHandler(&handler);
	parser.setErrorPrefix("Gxl");
	return parser.parse(fileName);
}
