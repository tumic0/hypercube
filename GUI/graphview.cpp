#include <QGraphicsScene>
#include <QWheelEvent>
#include <cmath>
#include "graphview.h"
#include "vertexitem.h"
#include "edgeitem.h"


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
	bool orig = painter->testRenderHint(QPainter::Antialiasing);

	painter->setRenderHint(QPainter::Antialiasing, false);
	QGraphicsRectItem::paint(painter, option, widget);
	painter->setRenderHint(QPainter::Antialiasing, orig);
}


GraphView::GraphView(QWidget *parent)
	: QGraphicsView(parent)
{
	_graphSize = 0;
	_dimensions = QPoint(0, 0);

	_scene = new QGraphicsScene(this);
	_boundingRect = new BoundingRectItem();
	_scene->addItem(_boundingRect);

	setScene(_scene);
}

GraphView::~GraphView()
{
	delete _scene;
}

void GraphView::clear()
{
	while (!_vertexList.isEmpty())
		delete _vertexList.takeFirst();
	while (!_edgeList.isEmpty())
		delete _edgeList.takeFirst();

	_graphSize = 0;
}

int GraphView::graphSize()
{
	return _graphSize;
}

VertexItem* GraphView::vertex(int id)
{
	return _vertexList.at(id);
}

VertexItem* GraphView::addVertex()
{
	VertexItem *v = new VertexItem();
	_scene->addItem(v);
	_vertexList.append(v);

	_graphSize++;

	return v;
}

EdgeItem* GraphView::edge(int src, int dst)
{
	VertexItem *v = _vertexList.at(src);

	for (int i = 0; i < v->edges().size(); i++)
		if (v->edges().at(i)->destVertex() == _vertexList.at(dst))
			return v->edges().at(i);

	return 0;
}

EdgeItem* GraphView::addEdge(int src, int dst)
{
	EdgeItem *e = new EdgeItem(_vertexList.at(src), _vertexList.at(dst));
	_scene->addItem(e);
	_edgeList.append(e);

	return e;
}

QPoint GraphView::dimensions(void) const
{
	return _dimensions;
}

void GraphView::setDimensions(const QPoint dimensions)
{
	QPoint offset = ((dimensions / 2) - (_dimensions / 2));
	for (int i = 0; i < _vertexList.size(); ++i)
		_vertexList.at(i)->moveBy(offset.x(), offset.y());


	_boundingRect->setRect(0, 0, dimensions.x() - 1,
	  dimensions.y() - 1);

	_scene->setSceneRect(0, 0, dimensions.x(), dimensions.y());
	_dimensions = dimensions;
}


void GraphView::setVertexColor(const QColor &color)
{
	for (int i = 0; i < _vertexList.size(); i++)
		_vertexList.at(i)->setColor(color);
}

void GraphView::setEdgeColor(const QColor &color)
{
	for (int i = 0; i < _edgeList.size(); i++)
		_edgeList.at(i)->setColor(color);
}

void GraphView::setVertexSize(int size)
{
	for (int i = 0; i < _vertexList.size(); i++)
		_vertexList.at(i)->setSize(size);
}

void GraphView::setEdgeSize(int size)
{
	for (int i = 0; i < _edgeList.size(); i++)
		_edgeList.at(i)->setSize(size);
}

void GraphView::setVertexFontSize(int size)
{
	for (int i = 0; i < _vertexList.size(); i++)
		_vertexList.at(i)->setFontSize(size);
}

void GraphView::setEdgeFontSize(int size)
{
	for (int i = 0; i < _edgeList.size(); i++)
		_edgeList.at(i)->setFontSize(size);
}

void GraphView::setEdgeZValue(int value)
{
	for (int i = 0; i < _edgeList.size(); i++)
		_edgeList.at(i)->setZValue(value);
}


void GraphView::wheelEvent(QWheelEvent *event)
{
	scale(pow(2.0, -event->delta() / 400.0));
	emit zoomed(transform().m11());
}

void GraphView::scale(qreal scaleFactor)
{
	qreal factor = matrix().scale(scaleFactor, scaleFactor)
	  .mapRect(QRectF(0, 0, 1, 1)).width();

	if (factor < 0.1 || factor > 10)
		return;

	QGraphicsView::scale(scaleFactor, scaleFactor);
}
