#include <string>
#include <map>
#include <deque>
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


class GxlHandler : public XmlHandler
{
public:
	GxlHandler(Graph *graph) : _graph(graph) {}
	void setNodeLabelAttribute(const wstring &name) {_nodeLabelAttr = name;}
	void setEdgeLabelAttribute(const wstring &name) {_edgeLabelAttr = name;}

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
	};

	struct EdgeAttributes {
		wstring id;
		wstring from;
		wstring to;
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
	wstring _nodeLabel, _edgeLabel;
	wstring _nodeLabelAttr, _edgeLabelAttr;

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

	if (element == NODE) {
		if (!(vertex = addVertex(_nodeAttributes.id)))
			return false;

		if (_nodeLabel.empty())
			vertex->setText(_nodeAttributes.id);
		else
			vertex->setText(_nodeLabel);

		clearNodeAttributes();
		_nodeLabel.clear();

	} else if (element == EDGE) {
		if (!(edge = addEdge(_edgeAttributes.from, _edgeAttributes.to)))
			return false;

		if (_edgeLabel.empty())
			edge->setText(_edgeAttributes.id);
		else
			edge->setText(_edgeLabel);

		clearEdgeAttributes();
		_edgeLabel.clear();
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
		if (isCompositeAttribute(parent)) {
			if (_attrAttributes.name == _nodeLabelAttr) {
				if (_nodeLabel.empty())
					_nodeLabel = data;
				else
					_nodeLabel += compositeDelimiter + data;
			}
			if (_attrAttributes.name == _edgeLabelAttr) {
				if (_edgeLabel.empty())
					_edgeLabel = data;
				else
					_edgeLabel += compositeDelimiter + data;
			}
		} else {
			if (_attrAttributes.name == _nodeLabelAttr)
				_nodeLabel = data;
			if (_attrAttributes.name == _edgeLabelAttr)
				_edgeLabel = data;
		}
	}

	return true;
}


void GxlGraphInput::setNodeLabelAttribute(const char *name)
{
	_nodeLabelAttr = s2w(name);
}

void GxlGraphInput::setEdgeLabelAttribute(const char *name)
{
	_edgeLabelAttr = s2w(name);
}

IO::Error GxlGraphInput::readGraph(Graph *graph, const char *fileName)
{
	GxlHandler handler(graph);
	XmlParser parser(&handler);

	handler.setNodeLabelAttribute(_nodeLabelAttr);
	handler.setEdgeLabelAttribute(_edgeLabelAttr);
	parser.setErrorPrefix("Gxl");

	return parser.parse(fileName);
}
