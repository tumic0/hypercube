#include <QVBoxLayout>
#include "CORE/vertex.h"
#include "CORE/edge.h"
#include "IO/modules.h"
#include "vertexitem.h"
#include "edgeitem.h"
#include "legenditem.h"
#include "graphtab.h"


GraphTab::GraphTab()
{
	_graph = new Graph();
	_view = new GraphView();
	_sa = new SA();

	_inputEncoding = 0;
	_inputProvider = 0;
	_outputProvider = 0;

	QVBoxLayout *layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(_view);
	setLayout(layout);

	_size = 50;
	_planarity = 50;
	_quality = 50;
}

GraphTab::~GraphTab()
{
	delete _view;
	delete _sa;
	delete _graph;
}

IO::Error GraphTab::readGraph(const QString &fileName)
{
	IO::Error error;

	QByteArray ba = fileName.toLocal8Bit();
	const char *cFileName = ba.data();

	for (InputProvider **p = inputProviders; *p; p++) {
		(*p)->setInputEncoding(_inputEncoding);

		error = (*p)->readGraph(_graph, cFileName);
		if (error == IO::Ok) {
			_inputProvider = *p;
			break;
		} else if (error != IO::FormatError)
			return error;
	}

	if (!_inputProvider)
		return IO::FormatError;

	_inputFileName = fileName;

	getGraphProperties();
	setGraphProperties();
	loadGraph();

	return IO::Ok;
}

IO::Error GraphTab::readGraph()
{
	_graph->clear();

	QByteArray ba = _inputFileName.toLocal8Bit();
	const char *cFileName = ba.data();

	_inputProvider->setInputEncoding(_inputEncoding);

	IO::Error error = _inputProvider->readGraph(_graph, cFileName);
	if (error != IO::Ok)
		return error;

	getGraphProperties();
	setGraphProperties();
	loadGraph();

	return IO::Ok;
}

IO::Error GraphTab::writeGraph(const QString &fileName, OutputProvider *provider)
{
	_outputProvider = provider;
	_outputFileName = fileName;

	return writeGraph();
}

IO::Error GraphTab::writeGraph(void)
{
	if (!_outputProvider)
		return IO::OpenError;
	else {
		storeGraph();
		QByteArray ba = _outputFileName.toLocal8Bit();
		const char *cFileName = ba.data();
		return _outputProvider->writeGraph(_graph, cFileName);
	}
}

Graph *GraphTab::graph()
{
	storeGraph();
	return _graph;
}

void GraphTab::transformGraph(void)
{
	storeGraph();
	_graph->randomize();
	_sa->compute(_graph);
	_graph->center();
	updateCoordinates();
}

void GraphTab::bindTo(Graph *graph)
{
	storeGraph();
	_graph->bindTo(graph);
	updateCoordinates();
}

void GraphTab::project(Graph* graph)
{
	storeGraph();
	_graph->project(graph);
	updateColors();
}

void GraphTab::colorizeEdges(void)
{
	_coloredEdges = true;

	storeGraph();
	_graph->colorize();

	for (size_t i = 0; i < _graph->edge_size(); i++) {
		Edge *edg = _graph->edge(i);
		EdgeItem *e = _view->edge(i);
		e->setColor(QColor(edg->color().rgb()));
	}
	updateLegend();
}

void GraphTab::setNodeLabelAttr(const QString &name)
{
	_view->setVertexLabelAttr(name);
	_nodeLabelAttr = name;
}

void GraphTab::setEdgeLabelAttr(const QString &name)
{
	_view->setEdgeLabelAttr(name);
	_edgeLabelAttr = name;
}

void GraphTab::setDimensions(const QPoint &dimensions)
{
	_view->setDimensions(dimensions);
	_dimensions = dimensions;
}

void GraphTab::setVertexColor(const QColor &color)
{
	_view->setVertexColor(color);
	_vertexColor = color;
}

void GraphTab::setEdgeColor(const QColor &color)
{
	_view->setEdgeColor(color);
	_edgeColor = color;
	_coloredEdges = false;
}

void GraphTab::setVertexSize(int size)
{
	_view->setVertexSize(size);
	_vertexSize = size;
}

void GraphTab::setEdgeSize(int size)
{
	_view->setEdgeSize(size);
	_edgeSize = size;
}

void GraphTab::setVertexFontSize(int size)
{
	_vertexFontSize = size;
	_view->setVertexFontSize(_showVertexIDs ? size : 0);
}

void GraphTab::setEdgeFontSize(int size)
{
	_edgeFontSize = size;
	_view->setEdgeFontSize(_showEdgeValues ? size : 0);
}

void GraphTab::showVertexIDs(bool show)
{
	_showVertexIDs = show;
	_view->setVertexFontSize(show ? _vertexFontSize : 0);
}

void GraphTab::showEdgeValues(bool show)
{
	_showEdgeValues = show;
	_view->setEdgeFontSize(show ? _edgeFontSize : 0);
}

void GraphTab::setLegend(int size)
{
	_legend = size;
	_view->setLegend(size);
}

bool GraphTab::directedGraph() const
{
	return _view->directedGraph();
}

void GraphTab::setDirectedGraph(bool state)
{
	_view->setDirectedGraph(state);
}

void GraphTab::setEdgeZValue(int value)
{
	_view->setEdgeZValue(value);
}

bool GraphTab::antialiasing() const
{
	return (_view->renderHints() & QPainter::Antialiasing)
	  ? true : false;
}

void GraphTab::setAntialiasing(bool value)
{
	_view->setRenderHint(QPainter::Antialiasing, value);
	_view->update();
}

void GraphTab::updateCoordinates()
{
	for (size_t i = 0; i < _graph->vertex_size(); i++) {
		Vertex *vtx = _graph->vertex(i);
		VertexItem *v = _view->vertex(i);
		v->setCoordinates(QPoint(vtx->coordinates().x(), vtx->coordinates().y()));
	}
}

void GraphTab::updateColors()
{
	for (size_t i = 0; i < _graph->vertex_size(); i++) {
		Vertex *vtx = _graph->vertex(i);
		VertexItem *v = _view->vertex(i);
		v->setColor(QColor(vtx->color().rgb()));
		v->setSize(vtx->size());
		v->setFontSize(vtx->fontSize());
	}

	for (size_t i = 0; i < _graph->edge_size(); i++) {
		Edge *edg = _graph->edge(i);
		EdgeItem *e = _view->edge(i);
		e->setColor(QColor(edg->color().rgb()));
		e->setSize(edg->size());
		e->setFontSize(edg->fontSize());
		e->setZValue(edg->zValue());
	}
}

void GraphTab::updateLegend()
{
	_view->clearLegend();

	for (ColorMap::const_iterator it = _graph->legend()->begin();
	  it != _graph->legend()->end(); it++) {
		LegendItem *l = _view->addLegend();

		l->setText(QString::fromStdWString((*it).first));
		l->setColor(QColor((*it).second.rgb()));
		l->setSize(_graph->legend()->size());
	}
}

void GraphTab::loadGraph()
{
	Vertex *vtx;
	Edge *edg;
	VertexItem *v;
	EdgeItem *e;


	_view->clear();

	_view->setDimensions(QPoint(_graph->dimensions().x(),
	  _graph->dimensions().y()));
	_view->setDirectedGraph(_graph->directed());
	_view->setLegend(_graph->legend()->size());


	for (size_t i = 0; i < _graph->vertex_size(); i++) {
		vtx = _graph->vertex(i);
		v = _view->addVertex();

		for (std::map<std::wstring, std::wstring>::const_iterator it
		  = vtx->attributes().begin(); it != vtx->attributes().end(); it++) {
			QString key = QString::fromStdWString(it->first);
			QString value = QString::fromStdWString(it->second);
			v->addAttribute(key, value);
		}
		v->setAttribute(QString::fromStdWString(vtx->attribute()));

		v->setSize(vtx->size());
		v->setCoordinates(QPoint(vtx->coordinates().x(), vtx->coordinates().y()));
		v->setColor(QColor(vtx->color().rgb()));
		v->setFontSize(vtx->fontSize());
	}

	for (size_t i = 0; i < _graph->edge_size(); i++) {
		edg = _graph->edge(i);
		e = _view->addEdge(_view->vertex(edg->src()->id()),
		  _view->vertex(edg->dst()->id()));

		for (std::map<std::wstring, std::wstring>::const_iterator it
		  = edg->attributes().begin(); it != edg->attributes().end(); it++) {
			QString key = QString::fromStdWString(it->first);
			QString value = QString::fromStdWString(it->second);

			e->addAttribute(key, value);
		}
		e->setAttribute(QString::fromStdWString(edg->attribute()));

		e->setColor(QColor(edg->color().rgb()));
		e->setSize(edg->size());
		e->setFontSize(edg->fontSize());
		e->setZValue(edg->zValue());
		e->setTwin(edg->twin());
	}

	updateLegend();
}

void GraphTab::storeGraph()
{
	Vertex *vtx;
	Edge *edg;
	VertexItem *v;
	EdgeItem *e;


	_graph->setDimensions(Coordinates(_view->dimensions().x(),
	  _view->dimensions().y()));
	_graph->setDirected(_view->directedGraph());
	_graph->legend()->setSize(_view->legend());

	for (int i = 0; i < _view->vertex_size(); i++) {
		v = _view->vertex(i);
		vtx = _graph->vertex(i);

		vtx->setAttribute(v->attribute().toStdWString());
		vtx->setCoordinates(Coordinates(v->coordinates().x(), v->coordinates().y()));
		vtx->setColor(v->color().rgb());
		vtx->setSize(v->size());
		vtx->setFontSize(v->fontSize());
	}

	for (int i = 0; i < _view->edge_size(); i++) {
		e = _view->edge(i);
		edg = _graph->edge(i);

		edg->setAttribute(e->attribute().toStdWString());
		edg->setColor(e->color().rgb());
		edg->setSize(e->size());
		edg->setFontSize(e->fontSize());
		edg->setZValue(e->zValue());
		edg->setTwin(e->twin());
	}
}

void GraphTab::setGraphProperties()
{
	_graph->setVertexAttribute(_nodeLabelAttr.toStdWString());
	_graph->setEdgeAttribute(_edgeLabelAttr.toStdWString());
	_graph->setVertexColor(_vertexColor.rgb());
	_graph->setVertexSize(_vertexSize);
	_coloredEdges ? _graph->colorize() : _graph->setEdgeColor(_edgeColor.rgb());
	_graph->setEdgeSize(_edgeSize);
	_graph->setVertexFontSize(_showVertexIDs ? _vertexFontSize : 0);
	_graph->setEdgeFontSize(_showEdgeValues ? _edgeFontSize : 0);
	_graph->legend()->setSize(_legend);

	_graph->setDimensions(Coordinates(_dimensions.x(), _dimensions.y()));
	_graph->randomize();
	_sa->compute(_graph);
	_graph->center();
}

void GraphTab::getGraphProperties()
{
	for (size_t i = 0; i < _graph->vertex_size(); i++) {
		Vertex *vtx = _graph->vertex(i);
		for (std::map<std::wstring, std::wstring>::const_iterator it
		  = vtx->attributes().begin(); it != vtx->attributes().end(); it++) {
			QString key = QString::fromStdWString(it->first);
			_nodeLabelAttributes.insert(key);
		}
	}
	for (size_t i = 0; i < _graph->edge_size(); i++) {
		Edge *edg = _graph->edge(i);
		for (std::map<std::wstring, std::wstring>::const_iterator it
		  = edg->attributes().begin(); it != edg->attributes().end(); it++) {
			QString key = QString::fromStdWString(it->first);
			_edgeLabelAttributes.insert(key);
		}
	}

	if (!_nodeLabelAttributes.empty())
		_nodeLabelAttr = *_nodeLabelAttributes.begin();
	if (!_edgeLabelAttributes.empty())
		_edgeLabelAttr = *_edgeLabelAttributes.begin();
}
