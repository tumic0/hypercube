#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include "edgeitem.h"
#include "vertexitem.h"
#include "CORE/config.h"


VertexItem::VertexItem()
{
	_color = QColor();
	_size = 0;
	_fontSize = 0;

	_text = new QGraphicsSimpleTextItem(QString::null, this);
	_text->setFont(QFont(FONT_FAMILY));

	setFlag(ItemIsMovable);
#if QT_VERSION >= 0x040600
	setFlag(ItemSendsGeometryChanges);
#endif
	setCacheMode(DeviceCoordinateCache);
}

VertexItem::~VertexItem()
{
	delete _text;
}

void VertexItem::addEdge(EdgeItem *edge)
{
	_edgeList.append(edge);
	edge->adjust();
}

void VertexItem::setSize(qreal size)
{
	qreal diff = (_size - size) / 2;
	_size = size;

	if (!size) {
		setVisible(false);
	} else {
		prepareGeometryChange();
		if (!isVisible())
			setVisible(true);
		moveBy(diff, diff);
		setRect(QRectF(0, 0, size, size));
		_text->setPos(size, size / 2);
	}
}

void VertexItem::setColor(const QColor &color)
{
	_color = color;
	setPen(QPen(Qt::NoPen));
	setBrush(QBrush(color));
	_text->setBrush(QBrush(color));
}

void VertexItem::setFontSize(int size)
{
	_fontSize = size;

	if (size <= 0) {
		_text->setVisible(false);
	} else {
		if (!_text->isVisible())
			_text->setVisible(true);
		QFont font = _text->font();
		font.setPixelSize(size);
		_text->setFont(font);
		_text->setPos(_size, _size / 2);
	}

	update(boundingRect());
}

QVariant VertexItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if (change == ItemPositionHasChanged)
		foreach (EdgeItem *edge, _edgeList)
			edge->adjust();

	return QGraphicsItem::itemChange(change, value);
}

void VertexItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	setPen(QPen(brush(), _size / 2));
	QGraphicsItem::mousePressEvent(event);
}

void VertexItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	setPen(QPen(Qt::NoPen));
	QGraphicsItem::mouseReleaseEvent(event);
}
