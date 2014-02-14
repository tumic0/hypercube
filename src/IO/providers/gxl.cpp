#include <string>
#include <map>
#include <deque>
#include <list>
#include "CORE/misc.h"
#include "IO/providers/xml/xml.h"
#include "gxl.h"

using namespace std;


#define ROOT            L""
#define GXL             L"gxl"
#define GRAPH           L"graph"
#define NODE            L"node"
#define EDGE            L"edge"
#define ATTR            L"attr"
#define BOOL            L"bool"
#define INT             L"int"
#define ENUM            L"enum"
#define FLOAT           L"float"
#define STRING          L"string"
#define SEQ             L"seq"
#define SET             L"set"
#define TUP             L"tup"
#define BAG             L"bag"
#define DIRECTED        L"directed"
#define DEFAULTDIRECTED L"defaultdirected"
#define ID              L"id"
#define FROM            L"from"
#define TO              L"to"
#define EDGEMODE        L"edgemode"
#define NAME            L"name"

#define compositeDelimiter L", "

#define isAtomicAttribute(element) \
	(element == INT || element == FLOAT || element == STRING \
	 || element == BOOL || element == ENUM)
#define isCompositeAttribute(element) \
	(element == SEQ || element == SET || element == TUP || element == BAG)

#define wstring_pair pair<wstring, wstring>
#define list_iterator std::list<wstring_pair >::iterator


class GxlHandler : public XmlHandler
{
public:
	GxlHandler(Graph *graph) : _graph(graph) {}

	virtual bool startDocument();
	virtual bool endDocument();
	virtual bool startElement(const wstring &name);
	virtual bool endElement(const wstring &name);
	virtual bool attribute(const wstring &name, const wstring &value);
	virtual bool data(const wstring &data);

private:
	struct Relation {
		const wstring node;
		const wstring * const parents;
		size_t numParents;
	};

	struct GraphAttributes {
		wstring edgemode;
	};

	struct NodeAttributes {
		wstring id;
		list<wstring_pair > attributes;
	};

	struct EdgeAttributes {
		wstring id;
		wstring from;
		wstring to;
		list<wstring_pair > attributes;
	};

	struct AttrAttributes {
		wstring name;
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
	bool handleData(const wstring &data);

	Graph *_graph;
	map<wstring, Vertex*> _vertexes;
	deque<wstring> _elements;
	GraphAttributes _graphAttributes;
	NodeAttributes _nodeAttributes;
	EdgeAttributes _edgeAttributes;
	AttrAttributes _attrAttributes;
	wstring _data;

	static const Relation relations[];
};


static const wstring gxlParents[] = {ROOT};
static const wstring graphParents[] = {GXL};
static const wstring nodeParents[] = {GRAPH};
#define edgeParents nodeParents
static const wstring atomicAttributesParents[] = {ATTR, SEQ, SET, TUP, BAG};
static const wstring compositeAttributesParents[] = {ATTR};

const GxlHandler::Relation GxlHandler::relations[] = {
	{GXL, gxlParents, ARRAY_SIZE(gxlParents)},
	{GRAPH, graphParents, ARRAY_SIZE(graphParents)},
	{NODE, nodeParents, ARRAY_SIZE(nodeParents)},
	{EDGE, edgeParents, ARRAY_SIZE(edgeParents)},
	{INT, atomicAttributesParents, ARRAY_SIZE(atomicAttributesParents)},
	{FLOAT, atomicAttributesParents, ARRAY_SIZE(atomicAttributesParents)},
	{STRING, atomicAttributesParents, ARRAY_SIZE(atomicAttributesParents)},
	{BOOL, atomicAttributesParents, ARRAY_SIZE(atomicAttributesParents)},
	{ENUM, atomicAttributesParents, ARRAY_SIZE(atomicAttributesParents)},
	{SEQ, compositeAttributesParents, ARRAY_SIZE(compositeAttributesParents)},
	{SET, compositeAttributesParents, ARRAY_SIZE(compositeAttributesParents)},
	{TUP, compositeAttributesParents, ARRAY_SIZE(compositeAttributesParents)},
	{BAG, compositeAttributesParents, ARRAY_SIZE(compositeAttributesParents)}
};


bool GxlHandler::startDocument()
{
	initGraphAttributes();

	return true;
}

bool GxlHandler::endDocument()
{
	bool directed = (_graphAttributes.edgemode == DIRECTED
	  || _graphAttributes.edgemode == DEFAULTDIRECTED) ? true : false;
	_graph->setDirected(directed);

	return true;
}

bool GxlHandler::startElement(const wstring &name)
{
	if (!checkRelation(name, _elements.empty() ? L"" : _elements.back()))
		return false;

	_elements.push_back(name);

	return true;
}

bool GxlHandler::endElement(const wstring &name)
{

	if (!handleElement(name))
		return false;

	_elements.pop_back();

	return true;
}

bool GxlHandler::attribute(const wstring &name, const wstring &value)
{
	setAttribute(_elements.back(), name, trim(value));

	return true;
}

bool GxlHandler::data(const wstring &data)
{
	wstring str = trim(data);
	if (str.empty())
		return true;
	else
		return handleData(str);
}


Vertex* GxlHandler::addVertex(const wstring &id)
{
	Vertex *v;
	map<wstring, Vertex*>::const_iterator it;

	if (id.empty())
		return 0;

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

	if (source.empty() || target.empty() || source == target)
		return 0;

	src = addVertex(source);
	dst = addVertex(target);

	return _graph->addEdge(src, dst);
}

bool GxlHandler::checkRelation(const wstring &node, const wstring &parent)
{
	if (parent.empty()) {
		if (node != GXL)
			return false;
	}

	for (size_t i = 0; i < ARRAY_SIZE(relations); i++) {
		if (node == relations[i].node) {
			for (size_t j = 0; j < relations[i].numParents; j++) {
				if (parent == relations[i].parents[j])
					return true;
			}
			return false;
		}
	}

	return true;
}

void GxlHandler::setAttribute(const wstring &element, const wstring &attr,
  const wstring &value)
{
	if (element == NODE) {
		if (attr == ID)
			_nodeAttributes.id = value;
	} else if (element == EDGE) {
		if (attr == ID)
			_edgeAttributes.id = value;
		if (attr == FROM)
			_edgeAttributes.from = value;
		if (attr == TO)
			_edgeAttributes.to = value;
	} else if (element == GRAPH) {
		if (attr == EDGEMODE)
			_graphAttributes.edgemode = value;
	} else if (element == ATTR) {
		if (attr == NAME)
			_attrAttributes.name = value;
	}
}

void GxlHandler::initGraphAttributes()
{
	_graphAttributes.edgemode = DIRECTED;
}

void GxlHandler::clearNodeAttributes()
{
	_nodeAttributes.id.clear();
	_nodeAttributes.attributes.clear();
}

void GxlHandler::clearEdgeAttributes()
{
	_edgeAttributes.id.clear();
	_edgeAttributes.from.clear();
	_edgeAttributes.to.clear();
	_edgeAttributes.attributes.clear();
}

bool GxlHandler::handleElement(const wstring &element)
{
	Vertex *vertex;
	Edge *edge;

	if (element == NODE) {
		if (!(vertex = addVertex(_nodeAttributes.id)))
			return false;

		for (list_iterator it = _nodeAttributes.attributes.begin();
		  it != _nodeAttributes.attributes.end(); it++)
			vertex->addAttribute(*it);
		vertex->addAttribute(wstring_pair(L"id", _nodeAttributes.id));

		clearNodeAttributes();
	} else if (element == EDGE) {
		if (!(edge = addEdge(_edgeAttributes.from, _edgeAttributes.to)))
			return false;

		for (list_iterator it = _edgeAttributes.attributes.begin();
		  it != _edgeAttributes.attributes.end(); it++)
			edge->addAttribute(*it);
		if (!_edgeAttributes.id.empty())
			edge->addAttribute(wstring_pair(L"id", _edgeAttributes.id));

		clearEdgeAttributes();
	} else if (element == ATTR) {
		const wstring &parent = _elements.at(_elements.size() - 2);
		if (parent == NODE)
			_nodeAttributes.attributes.push_back(
			  wstring_pair(_attrAttributes.name, _data));
		if (parent == EDGE)
			_edgeAttributes.attributes.push_back(
			  wstring_pair(_attrAttributes.name, _data));
		_data.clear();
	}

	return true;
}

bool GxlHandler::handleData(const wstring &data)
{
	if (_elements.size() < 2)
		return true;

	const wstring &element = _elements.back();
	const wstring &parent = _elements.at(_elements.size() - 2);

	if (isAtomicAttribute(element)) {
		if (isCompositeAttribute(parent) && !_data.empty())
			_data += compositeDelimiter + data;
		else
			_data = data;
	}

	return true;
}

IO::Error GxlGraphInput::readGraph(Graph *graph, const char *fileName)
{
	GxlHandler handler(graph);
	XmlParser parser(&handler);

	parser.setErrorPrefix("Gxl");
	return parser.parse(fileName);
}
