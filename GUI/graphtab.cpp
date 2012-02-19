#include <QtGui>
#include <QFileInfo>
#include "CORE/vertex.h"
#include "CORE/edge.h"
#include "IO/modules.h"
#include "vertexitem.h"
#include "edgeitem.h"
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

	InputProvider **p = inputProviders;
	while (*p) {
		error = (*p)->readGraph(_graph, cFileName, _inputEncoding);
		if (error == IO::Ok) {
			_inputProvider = *p;
			break;
		} else if (error != IO::FormatError)
			return error;
		p++;
	}

	if (!_inputProvider)
		return IO::FormatError;

	_inputFileName = fileName;

	setGraphProperties();
	loadGraph();

	return IO::Ok;
}

IO::Error GraphTab::readGraph()
{
	_graph->clear();

	QByteArray ba = _inputFileName.toLocal8Bit();
	const char *cFileName = ba.data();

	IO::Error error = _inputProvider->readGraph(_graph, cFileName,
	  _inputEncoding);
	if (error != IO::Ok)
		return error;

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
	_sa->compute(_graph);
	_graph->center();
	loadGraph();
}

void GraphTab::bindTo(Graph *graph)
{
	storeGraph();
	_graph->bindTo(graph);
	loadGraph();
}

void GraphTab::project(Graph* graph)
{
	storeGraph();
	_graph->project(graph);
	loadGraph();
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

void GraphTab::colorizeEdges(bool colorize)
{
	_coloredEdges = colorize;
	if (colorize) {
		storeGraph();
		_graph->colorize();
		loadGraph();
	} else
		_view->setEdgeColor(_edgeColor);
}

void GraphTab::setEdgeZValue(int value)
{
	_view->setEdgeZValue(value);
}

void GraphTab::setInputEncoding(Encoding *encoding)
{
	_inputEncoding = encoding;
	if (_inputProvider)
		readGraph();
}

bool GraphTab::antialiasing()
{
	return (_view->renderHints() & QPainter::Antialiasing)
	  ? true : false;
}

void GraphTab::setAntialiasing(bool value)
{
	QPainter::RenderHints hints = _view->renderHints();
	hints = (value) ? hints | QPainter::Antialiasing
	  : hints & ~QPainter::Antialiasing;

	_view->setRenderHints(hints);
}


void GraphTab::loadGraph()
{
	Vertex *vtx;
	Edge *edg;
	VertexItem *v;
	EdgeItem *e;
	Coordinates c;


	_view->clear();

	_view->setDimensions(QPoint(_graph->dimensions().x(),
	  _graph->dimensions().y()));

	for (size_t i = 0; i < _graph->vertex_size(); i++) {
		vtx = _graph->vertex(i);
		v = _view->addVertex();

		c = vtx->coordinates();
		v->setColor(QColor(vtx->color().rgb()));
		v->setSize(vtx->size());
		v->setText(QString::fromStdWString(vtx->text()));
		v->setFontSize(vtx->fontSize());
		v->setPos(c.x(), c.y());
	}

	for (size_t i = 0; i < _graph->edge_size(); i++) {
		edg = _graph->edge(i);
		e = _view->addEdge(_view->vertex(edg->src()->id()),
		  _view->vertex(edg->dst()->id()));

		e->setColor(QColor(edg->color().rgb()));
		e->setSize(edg->size());
		e->setText(QString::fromStdWString(edg->text()));
		e->setFontSize(edg->fontSize());
		e->setZValue(edg->zValue());
	}
}

void GraphTab::storeGraph()
{
	Vertex *vtx;
	Edge *edg;
	VertexItem *v;
	EdgeItem *e;
	QPointF pos;


	_graph->clear();

	_graph->setDimensions(Coordinates(_view->dimensions().x(),
	  _view->dimensions().y()));

	for (int i = 0; i < _view->vertex_size(); i++) {
		v = _view->vertex(i);
		pos = v->scenePos();
		vtx = _graph->addVertex();

		vtx->setCoordinates(Coordinates(pos.x(), pos.y()));
		vtx->setColor(v->color().rgb());
		vtx->setSize(v->size());
		vtx->setText(v->text().toStdWString());
		vtx->setFontSize(v->fontSize());
	}

	for (int i = 0; i < _view->edge_size(); i++) {
		e = _view->edge(i);
		edg = _graph->addEdge(_graph->vertex(e->src()->id()),
		  _graph->vertex(e->dst()->id()));

		edg->setColor(e->color().rgb());
		edg->setSize(e->size());
		edg->setText(e->text().toStdWString());
		edg->setFontSize(e->fontSize());
		edg->setZValue(e->zValue());
	}
}

void GraphTab::setGraphProperties()
{
	_graph->setVertexColor(_vertexColor.rgb());
	_graph->setVertexSize(_vertexSize);
	if (_coloredEdges)
		_graph->colorize();
	else
		_graph->setEdgeColor(_edgeColor.rgb());
	_graph->setEdgeSize(_edgeSize);
	_graph->setVertexFontSize(_showVertexIDs ? _vertexFontSize : 0);
	_graph->setEdgeFontSize(_showEdgeValues ? _edgeFontSize : 0);

	_graph->setDimensions(Coordinates(_dimensions.x(), _dimensions.y()));
	_graph->randomize();
}
