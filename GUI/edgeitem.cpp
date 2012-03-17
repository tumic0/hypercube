#include <QPainter>
#include <cmath>
#include "CORE/misc.h"
#include "CORE/config.h"
#include "vertexitem.h"
#include "edgeitem.h"


const float Pi = 3.141592;
const float C1 = 0.866025; /* sqrt(3)/2 */

EdgeItem::EdgeItem(VertexItem *src, VertexItem *dst)
{
	_directed = true;
	_size = 0;
	_fontSize = 0;

	_text.setParentItem(this);
	_text.setFont(QFont(FONT_FAMILY));

	setZValue(-1);

	_src = src;
	_dst = dst;

	_src->addEdge(this);
	_dst->addEdge(this);

	adjust();

	setCacheMode(DeviceCoordinateCache);
}

void EdgeItem::adjust()
{
	prepareGeometryChange();
	_text.setPos(textPos());
}

void EdgeItem::setDirected(bool val)
{
	_directed = val;
	update();
}

void EdgeItem::setSize(qreal size)
{
	_size = size;

	if (!_size) {
		setVisible(false);
	} else {
		if (!isVisible())
			setVisible(true);
		_text.setPos(textPos());
	}

	update();
}

void EdgeItem::setColor(const QColor &color)
{
	_color = color;
	_text.setBrush(QBrush(color));

	update();
}

void EdgeItem::setFontSize(int size)
{
	_fontSize = size;

	if (size <= 0) {
		_text.setVisible(false);
	} else {
		if (!_text.isVisible())
			_text.setVisible(true);
		QFont font = _text.font();
		font.setPixelSize(size);
		_text.setFont(font);
		_text.setPos(textPos());
	}

	update();
}

QLineF EdgeItem::edgeLine()
{
	QPointF src, dst;
	src = _src->coordinates() + QPointF(_src->size() / 2, _src->size() / 2);
	dst = _dst->coordinates() + QPointF(_dst->size() / 2, _dst->size() / 2);

	return QLineF(src, dst);
}

QPointF EdgeItem::textPos()
{
	QLineF line = edgeLine();

	if ((line.dx() > 0 && line.dy() > 0) || (line.dx() < 0 && line.dy() < 0))
		return (line.pointAt(0.5) + QPointF(_size / 2, - _size / 2)
		  - QPointF(0, _text.font().pixelSize()));
	else
		return (line.pointAt(0.5) + QPointF(_size / 2, _size / 2));
}


QRectF EdgeItem::boundingRect() const
{
	qreal inc = qMax(_src->size(), _dst->size());
	QRectF rect = QRectF(_src->coordinates(), _dst->coordinates());

	return rect.normalized().adjusted(-inc, -inc, inc, inc);
}

void EdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
  QWidget *)
{
	QLineF line = edgeLine();

	if (_directed) {
		QPointF arrow[3];
		float angle;

		angle = acos(line.dx() / line.length());
		if (line.dy() >= 0)
			angle = 2.0 * Pi - angle;

		arrow[0] = line.pointAt(1 - ((_dst->size() / 2.0) / line.length()));
		arrow[1] = arrow[0] + QPointF(sin(angle - Pi / 3) * _dst->size(),
		  cos(angle - Pi / 3) * _dst->size());
		arrow[2] = arrow[0] + QPointF(sin(angle - Pi + Pi / 3) * _dst->size(),
		  cos(angle - Pi + Pi / 3) * _dst->size());

		line.setP2(line.pointAt(1 - ((_dst->size() * C1 * 1.5) / line.length())));

		painter->setPen(QPen(Qt::NoPen));
		painter->setBrush(_color);
		painter->drawPolygon(QPolygonF() << arrow[0] << arrow[1] << arrow[2]);
	}

	painter->setPen(QPen(_color, _size, Qt::SolidLine, Qt::FlatCap,
	  Qt::BevelJoin));
	painter->drawLine(line);
}
