#include <QGraphicsScene>
#include <QWheelEvent>
#include <cmath>
#include "graphview.h"
#include "vertexitem.h"
#include "edgeitem.h"
#include "legenditem.h"

#include <iostream>

class BoundingRectItem : public QGraphicsRectItem
{
public:
	BoundingRectItem(QGraphicsItem *parent = 0) : QGraphicsRectItem(parent)
	  {setPen(Qt::DashLine);}
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
	  QWidget *widget);
};

void BoundingRectItem::paint(QPainter *painter,
  const QStyleOptionGraphicsItem *option, QWidget *widget = 0)
{
	bool antialiasing = painter->testRenderHint(QPainter::Antialiasing);

	if (antialiasing) {
		painter->setRenderHint(QPainter::Antialiasing, false);
		QGraphicsRectItem::paint(painter, option, widget);
		painter->setRenderHint(QPainter::Antialiasing, true);
	} else
		QGraphicsRectItem::paint(painter, option, widget);
}


GraphView::GraphView(QWidget *parent)
	: QGraphicsView(parent)
{
	_dimensions = QPoint(0, 0);
	_boundingRect = 0;

	_directed = false;
	_legend = 0;

	_scene = new QGraphicsScene(this);
	setScene(_scene);
}

GraphView::~GraphView()
{
	delete _scene;
}

void GraphView::clear()
{
	_scene->clear();

	_vertexes.clear();
	_edges.clear();
	_legendItems.clear();

	_boundingRect = 0;
}

void GraphView::clearLegend()
{
	for (int i = 0; i < _legendItems.size(); i++)
		delete _legendItems.at(i);
	_legendItems.clear();
}

VertexItem* GraphView::addVertex()
{
	VertexItem *v = new VertexItem(_vertexes.size());
	_scene->addItem(v);
	_vertexes.append(v);

	return v;
}

EdgeItem* GraphView::addEdge(VertexItem *src, VertexItem *dst)
{
	EdgeItem *e = new EdgeItem(src, dst);
	e->setDirected(_directed);
	_scene->addItem(e);
	_edges.append(e);

	return e;
}

LegendItem* GraphView::addLegend()
{
	LegendItem *l = new LegendItem(_legendItems.count());
	_scene->addItem(l);
	_legendItems.append(l);

	return l;
}

void GraphView::setDimensions(const QPoint dimensions)
{
	QPoint offset = ((dimensions / 2) - (_dimensions / 2));
	for (int i = 0; i < _vertexes.size(); ++i)
		_vertexes.at(i)->moveBy(offset.x(), offset.y());


	if (!_boundingRect) {
		_boundingRect = new BoundingRectItem();
		_scene->addItem(_boundingRect);
	}

	_boundingRect->setRect(0, 0, dimensions.x() - 1,
	  dimensions.y() - 1);

	_scene->setSceneRect(0, 0, dimensions.x(), dimensions.y());
	_dimensions = dimensions;
}

void GraphView::setLegend(int size)
{
	_legend = size;

	for (int i = 0; i < _legendItems.size(); i++)
		_legendItems.at(i)->setSize(size);
}

void GraphView::setDirectedGraph(bool state)
{
	_directed = state;

	for (int i = 0; i < _edges.size(); i++)
		_edges.at(i)->setDirected(state);
}

void GraphView::setVertexLabelAttr(const QString &name)
{
	for (int i = 0; i < _vertexes.size(); i++)
		_vertexes.at(i)->setAttribute(name);
}

void GraphView::setEdgeLabelAttr(const QString &name)
{
	for (int i = 0; i < _edges.size(); i++)
		_edges.at(i)->setAttribute(name);
}

void GraphView::setVertexColor(const QColor &color)
{
	for (int i = 0; i < _vertexes.size(); i++)
		_vertexes.at(i)->setColor(color);
}

void GraphView::setEdgeColor(const QColor &color)
{
	for (int i = 0; i < _edges.size(); i++)
		_edges.at(i)->setColor(color);
}

void GraphView::setVertexSize(int size)
{
	for (int i = 0; i < _vertexes.size(); i++)
		_vertexes.at(i)->setSize(size);
}

void GraphView::setEdgeSize(int size)
{
	for (int i = 0; i < _edges.size(); i++)
		_edges.at(i)->setSize(size);
}

void GraphView::setVertexFontSize(int size)
{
	for (int i = 0; i < _vertexes.size(); i++)
		_vertexes.at(i)->setFontSize(size);
}

void GraphView::setEdgeFontSize(int size)
{
	for (int i = 0; i < _edges.size(); i++)
		_edges.at(i)->setFontSize(size);
}

void GraphView::setEdgeZValue(int value)
{
	for (int i = 0; i < _edges.size(); i++)
		_edges.at(i)->setZValue(value);
}


void GraphView::wheelEvent(QWheelEvent *event)
{
	scale(pow(2.0, -event->angleDelta().y() / 400.0));
	emit zoomed(transform().m11());
}

void GraphView::scale(qreal scaleFactor)
{
	qreal factor = transform().scale(scaleFactor, scaleFactor)
	  .mapRect(QRectF(0, 0, 1, 1)).width();

	if (factor < 0.1 || factor > 10)
		return;

	QGraphicsView::scale(scaleFactor, scaleFactor);
}
