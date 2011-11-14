#include <QtGui>
#include <QFileInfo>
#include "vertexitem.h"
#include "edgeitem.h"
#include "IO/modules.h"
#include "graphtab.h"


/*!
	\class GraphTab
	\brief GraphView to Graph bonding class

	Lorem ipsum...
*/

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
	VertexItem *v;
	EdgeItem *e;
	Coordinates c;

	/* Clear the old graph if any */
	_view->clear();

	/* Add the new graph */
	_view->setDimensions(QPoint(_graph->dimensions().x(),
	  _graph->dimensions().y()));

	for (int i = 0; i < _graph->size(); i++) {
		v = _view->addVertex();

		c = _graph->vertexCoordinates(i);
		v->setColor(QColor(_graph->vertexColor(i).rgb()));
		v->setSize(_graph->vertexSize(i));
		v->setText(QString::fromStdWString(_graph->vertexText(i)));
		v->setFontSize(_graph->vertexFontSize(i));
		v->setPos(c.x(), c.y());
	}

	for (int i = 0; i < _graph->size(); i++) {
		for (int j = 0; j < i; j++) {
			if (_graph->edge(i, j)) {
				e = _view->addEdge(i, j);

				e->setColor(QColor(_graph->edgeColor(i, j).rgb()));
				e->setSize(_graph->edgeSize(i, j));
				e->setText(QString::fromStdWString(
				  _graph->edgeText(i, j)));
				e->setFontSize(_graph->edgeFontSize(i, j));
				e->setZValue(_graph->edgeZValue(i, j));
			}
		}
	}
}

void GraphTab::storeGraph()
{
	VertexItem *v;
	EdgeItem *e;
	QPointF pos;

	/* Clear the old graph if any */
	_graph->clear();

	/* Add the new graph */
	_graph->setDimensions(Coordinates(_view->dimensions().x(),
	  _view->dimensions().y()));

	for (int i = 0; i < _view->graphSize(); i++) {
		v = _view->vertex(i);
		pos = v->scenePos();
		_graph->addVertex();

		_graph->moveVertex(i, Coordinates(pos.x(), pos.y()));
		_graph->setVertexColor(i, v->color().rgb());
		_graph->setVertexSize(i, v->size());
		_graph->setVertexFontSize(i, v->fontSize());
	}

	for (int i = 0; i < _view->graphSize(); i++) {
		for (int j = 0; j < i; j++) {
			if ((e = _view->edge(i, j))) {
				_graph->addEdge(i, j);

				_graph->setEdgeColor(i, j, e->color().rgb());
				_graph->setEdgeSize(i, j, e->size());
				_graph->setEdgeText(i, j, e->text().toStdWString());
				_graph->setEdgeFontSize(i, j, e->fontSize());
				_graph->setEdgeZValue(i, j, e->zValue());
			}
		}
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
