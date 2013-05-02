#include <string>
#include <map>
#include <deque>
#include "CORE/misc.h"
#include "CORE/config.h"
#include "IO/providers/xml/xml.h"
#include "graphml.h"

using namespace std;


class GraphmlHandler : public XmlHandler
{
public:
	GraphmlHandler(Graph *graph) : _graph(graph) {}

	virtual bool startDocument();
	virtual bool endDocument();
	virtual bool startElement(const wstring &name);
	virtual bool endElement(const wstring &name);
	virtual bool attribute(const wstring &name, const wstring &value);
	virtual bool data(const wstring &data);

private:
	struct Relation {
		wstring node;
		wstring parent;
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

	static const Relation relations[];
};


const GraphmlHandler::Relation GraphmlHandler::relations[] = {
	{L"graphml", L""},
	{L"graph", L"graphml"},
	{L"node", L"graph"},
	{L"edge", L"graph"},
	{L"key", L"graphml"},
	{L"default", L"key"}
};


bool GraphmlHandler::startDocument()
{
	initGraphAttributes();

	return true;
}

bool GraphmlHandler::endDocument()
{
	_graph->setDirected((_graphAttributes.edgedefault == L"directed"));

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

	src = addVertex(source);
	dst = addVertex(target);

	return _graph->addEdge(src, dst);
}

bool GraphmlHandler::checkRelation(const wstring &node, const wstring &parent)
{
	if (parent.empty()) {
		if (node != L"graphml")
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
	if (element == L"node") {
		if (attr == L"id")
			_nodeAttributes.id = value;
	} else if (element == L"edge") {
		if (attr == L"id")
			_edgeAttributes.id = value;
		else if (attr == L"source")
			_edgeAttributes.source = value;
		else if (attr == L"target")
			_edgeAttributes.target = value;
	} else if (element == L"graph") {
		if (attr == L"edgedefault")
			_graphAttributes.edgedefault = value;
	} else if (element == L"data") {
		if (attr == L"key")
			_dataAttributes.key = value;
	} else if (element == L"key") {
		if (attr == L"id")
			_keyAttributes.id = value;
		else if (attr == L"for")
			_keyAttributes.xfor = value;
		else if (attr == L"attr.name")
			_keyAttributes.name =value;
	}
}

void GraphmlHandler::initGraphAttributes()
{
	_graphAttributes.edgedefault = L"directed";
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

	if (element == L"node") {
		if (_nodeAttributes.id.empty())
			return false;
		vertex = addVertex(_nodeAttributes.id);

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

	} else if (element == L"edge") {
		if (_edgeAttributes.source.empty() || _edgeAttributes.target.empty())
			return false;
		edge = addEdge(_edgeAttributes.source, _edgeAttributes.target);

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

	} else if (element == L"key") {
		if (_keyAttributes.xfor == L"node"
		  && _keyAttributes.name == NODE_LABEL_ATTR)
			_nodeLabel.id = _keyAttributes.id;
		else if (_keyAttributes.xfor == L"edge"
		  && _keyAttributes.name == EDGE_LABEL_ATTR)
			_edgeLabel.id = _keyAttributes.id;

		clearKeyAttributes();
	}

	return true;
}

bool GraphmlHandler::handleData(const wstring &data)
{
	if (_elements.back() == L"data") {
		wstring &parent = _elements.at(_elements.size() - 2);

		if (parent == L"node" && _dataAttributes.key == _nodeLabel.id)
			_nodeLabel.value = data;
		if (parent == L"edge" && _dataAttributes.key == _edgeLabel.id)
			_edgeLabel.value = data;
	} else if (_elements.back() == L"default") {
		if (_keyAttributes.xfor == L"node")
			_nodeLabel.defval = data;
		else if (_keyAttributes.xfor == L"edge")
			_edgeLabel.defval = data;
	}

	return true;
}


IO::Error GraphmlGraphInput::readGraph(Graph *graph, const char *fileName,
  Encoding *)
{
	XmlParser parser;
	GraphmlHandler handler(graph);

	parser.setHandler(&handler);
	parser.setErrorPrefix("GraphML");
	return parser.parse(fileName);
}
