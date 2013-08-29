#include <string>
#include <map>
#include <deque>
#include "CORE/misc.h"
#include "IO/providers/xml/xml.h"
#include "graphml.h"

using namespace std;


#define ROOT        L""
#define GRAPHML     L"graphml"
#define GRAPH       L"graph"
#define NODE        L"node"
#define EDGE        L"edge"
#define KEY         L"key"
#define DEFAULT     L"default"
#define EDGEDEFAULT L"edgedefault"
#define DIRECTED    L"directed"
#define NAME        L"attr.name"
#define ID          L"id"
#define SOURCE      L"source"
#define TARGET      L"target"
#define DATA        L"data"
#define FOR         L"for"


class GraphmlHandler : public XmlHandler
{
public:
	GraphmlHandler(Graph *graph) : _graph(graph) {}
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
		const wstring parent;
	};

	struct GraphAttributes {
		wstring edgedefault;
	};

	struct NodeAttributes {
		wstring id;
		wstring label;
	};

	struct EdgeAttributes {
		wstring id;
		wstring source;
		wstring target;
		wstring label;
	};

	struct KeyAttributes {
		wstring id;
		wstring xfor;
		wstring name;
	};

	struct DataAttributes {
		wstring key;
	};

	struct Label {
		wstring id;
		wstring defval;
		wstring value;
	};

	Vertex *addVertex(const wstring &id);
	Edge *addEdge(const wstring &source, const wstring &target);
	bool checkRelation(const wstring &node, const wstring &parent);
	void initGraphAttributes();
	void clearNodeAttributes();
	void clearEdgeAttributes();
	void clearKeyAttributes();
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
	DataAttributes _dataAttributes;
	KeyAttributes _keyAttributes;
	Label _nodeLabel, _edgeLabel;
	wstring _nodeLabelAttr, _edgeLabelAttr;

	static const Relation relations[];
};


const GraphmlHandler::Relation GraphmlHandler::relations[] = {
	{GRAPHML, ROOT},
	{GRAPH, GRAPHML},
	{KEY, GRAPHML},
	{NODE, GRAPH},
	{EDGE, GRAPH},
	{DEFAULT, KEY}
};


bool GraphmlHandler::startDocument()
{
	initGraphAttributes();

	return true;
}

bool GraphmlHandler::endDocument()
{
	_graph->setDirected((_graphAttributes.edgedefault == DIRECTED));

	return true;
}

bool GraphmlHandler::startElement(const wstring &name)
{
	if (!checkRelation(name, _elements.empty() ? L"" : _elements.back()))
		return false;

	_elements.push_back(name);

	return true;
}

bool GraphmlHandler::endElement(const wstring &)
{
	if (!handleElement(_elements.back()))
		return false;

	_elements.pop_back();

	return true;
}

bool GraphmlHandler::attribute(const wstring &name, const wstring &value)
{
	setAttribute(_elements.back(), name, value);

	return true;
}

bool GraphmlHandler::data(const wstring &data)
{
	wstring str = trim(data);
	if (str.empty())
		return true;
	else
		return handleData(str);
}


Vertex* GraphmlHandler::addVertex(const wstring &id)
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

Edge* GraphmlHandler::addEdge(const wstring &source, const wstring &target)
{
	Vertex *src, *dst;

	if (source.empty() || target.empty() || source == target)
		return 0;

	src = addVertex(source);
	dst = addVertex(target);

	return _graph->addEdge(src, dst);
}

bool GraphmlHandler::checkRelation(const wstring &node, const wstring &parent)
{
	if (parent.empty()) {
		if (node != GRAPHML)
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

void GraphmlHandler::setAttribute(const wstring &element, const wstring &attr,
  const wstring &value)
{
	if (element == NODE) {
		if (attr == ID)
			_nodeAttributes.id = value;
	} else if (element == EDGE) {
		if (attr == ID)
			_edgeAttributes.id = value;
		else if (attr == SOURCE)
			_edgeAttributes.source = value;
		else if (attr == TARGET)
			_edgeAttributes.target = value;
	} else if (element == GRAPH) {
		if (attr == EDGEDEFAULT)
			_graphAttributes.edgedefault = value;
	} else if (element == DATA) {
		if (attr == KEY)
			_dataAttributes.key = value;
	} else if (element == KEY) {
		if (attr == ID)
			_keyAttributes.id = value;
		else if (attr == FOR)
			_keyAttributes.xfor = value;
		else if (attr == NAME)
			_keyAttributes.name =value;
	}
}

void GraphmlHandler::initGraphAttributes()
{
	_graphAttributes.edgedefault = DIRECTED;
}

void GraphmlHandler::clearNodeAttributes()
{
	_nodeAttributes.id.clear();
}

void GraphmlHandler::clearEdgeAttributes()
{
	_edgeAttributes.id.clear();
	_edgeAttributes.source.clear();
	_edgeAttributes.target.clear();
}

void GraphmlHandler::clearKeyAttributes()
{
	_keyAttributes.id.clear();
	_keyAttributes.xfor.clear();
	_keyAttributes.name.clear();
}

bool GraphmlHandler::handleElement(const wstring &element)
{
	Vertex *vertex;
	Edge *edge;

	if (element == NODE) {
		if (!(vertex = addVertex(_nodeAttributes.id)))
			return false;

		if (_nodeLabel.id.empty())
			vertex->setText(_nodeAttributes.id);
		else {
			if (!_nodeLabel.value.empty())
				vertex->setText(_nodeLabel.value);
			else
				vertex->setText(_nodeLabel.defval);
		}

		clearNodeAttributes();
		_nodeLabel.value.clear();

	} else if (element == EDGE) {
		if (!(edge = addEdge(_edgeAttributes.source, _edgeAttributes.target)))
			return false;

		if (_edgeLabel.id.empty())
			edge->setText(_edgeAttributes.id);
		else {
			if (!_edgeLabel.value.empty())
				edge->setText(_edgeLabel.value);
			else
				edge->setText(_edgeLabel.defval);
		}

		clearEdgeAttributes();
		_edgeLabel.value.clear();

	} else if (element == KEY) {
		if (_keyAttributes.xfor == NODE
		  && _keyAttributes.name == _nodeLabelAttr)
			_nodeLabel.id = _keyAttributes.id;
		else if (_keyAttributes.xfor == EDGE
		  && _keyAttributes.name == _edgeLabelAttr)
			_edgeLabel.id = _keyAttributes.id;

		clearKeyAttributes();
	}

	return true;
}

bool GraphmlHandler::handleData(const wstring &data)
{
	if (_elements.back() == DATA) {
		wstring &parent = _elements.at(_elements.size() - 2);

		if (parent == NODE && _dataAttributes.key == _nodeLabel.id)
			_nodeLabel.value = data;
		if (parent == EDGE && _dataAttributes.key == _edgeLabel.id)
			_edgeLabel.value = data;
	} else if (_elements.back() == DEFAULT) {
		if (_keyAttributes.xfor == NODE)
			_nodeLabel.defval = data;
		else if (_keyAttributes.xfor == EDGE)
			_edgeLabel.defval = data;
	}

	return true;
}


void GraphmlGraphInput::setNodeLabelAttribute(const char *name)
{
	_nodeLabelAttr = s2w(name);
}

void GraphmlGraphInput::setEdgeLabelAttribute(const char *name)
{
	_edgeLabelAttr = s2w(name);
}

IO::Error GraphmlGraphInput::readGraph(Graph *graph, const char *fileName)
{
	GraphmlHandler handler(graph);
	XmlParser parser(&handler);

	handler.setNodeLabelAttribute(_nodeLabelAttr);
	handler.setEdgeLabelAttribute(_edgeLabelAttr);
	parser.setErrorPrefix("GraphML");

	return parser.parse(fileName);
}
