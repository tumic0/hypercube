#include <QPainter>
#include <QtCore/qmath.h>
#include "CORE/config.h"
#include "vertexitem.h"
#include "edgeitem.h"


const float Pi = 3.141592f;
const float C1 = 0.866025f; /* sqrt(3)/2 */


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
}

void EdgeItem::adjust()
{
	prepareGeometryChange();
	_text.setPos(textPos());
}

void EdgeItem::setAttribute(const QString &attribute)
{
	if (_attributes.contains(attribute)) {
		_text.setText(_attributes[attribute]);
		_attribute = attribute;
	} else {
		_text.setText(QString());
		_attribute.clear();
	}
}

void EdgeItem::addAttribute(const QString &attribute, const QString &value)
{
	_attributes.insert(attribute, value);
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
		_text.setPos(textPos());
		if (!isVisible())
			setVisible(true);
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
		QFont font = _text.font();
		font.setPixelSize(size);
		_text.setFont(font);
		_text.setPos(textPos());

		if (!_text.isVisible())
			_text.setVisible(true);
	}

	update();
}

QLineF EdgeItem::edgeLine()
{
	QPointF src, dst;
	src = _src->pos() + QPointF(_src->size() / 2, _src->size() / 2);
	dst = _dst->pos() + QPointF(_dst->size() / 2, _dst->size() / 2);

	return QLineF(src, dst);
}

QPointF EdgeItem::textPos()
{
	QLineF line = edgeLine();

	qreal angle = qAtan2(qAbs(line.dy()), qAbs(line.dx()));
	qreal h = _text.boundingRect().height() / 2;
	qreal w = _text.boundingRect().width() / 2;
	qreal hyp = qSqrt(h*h + w*w);
	qreal dist = hyp * qSin(angle + qAsin(h / hyp));

	QLineF l(line);
	l.setP1(l.pointAt(0.5));
	l = l.normalVector();
	l.setLength((_size / 2) + dist);

	return l.p2() - QPointF(w, h);
}


QRectF EdgeItem::boundingRect() const
{
	qreal inc = qMax(_src->size(), _dst->size());
	QRectF rect = QRectF(_src->pos(), _dst->pos());

	return rect.normalized().adjusted(-inc, -inc, inc, inc);
}

void EdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
  QWidget *)
{
	QLineF line = edgeLine();

	if (_directed) {
		QPointF arrow[3];
		float angle;

		if (_twin)
			line.setP1(line.pointAt(0.5));

		angle = qAtan2(-line.dy(), line.dx());
		if (line.dy() >= 0)
			angle = 2.0 * Pi + angle;

		arrow[0] = line.pointAt(1 - ((_dst->size() / 2.0) / line.length()));
		arrow[1] = arrow[0] + QPointF(qSin(angle - Pi / 3) * _dst->size(),
		  qCos(angle - Pi / 3) * _dst->size());
		arrow[2] = arrow[0] + QPointF(qSin(angle - Pi + Pi / 3) * _dst->size(),
		  qCos(angle - Pi + Pi / 3) * _dst->size());

		line.setP2(line.pointAt(1 - ((_dst->size() * C1 * 1.5) / line.length())));

		painter->setPen(QPen(Qt::NoPen));
		painter->setBrush(_color);
		painter->drawPolygon(QPolygonF() << arrow[0] << arrow[1] << arrow[2]);
	}

	painter->setPen(QPen(_color, _size, Qt::SolidLine, Qt::FlatCap,
	  Qt::BevelJoin));
	painter->drawLine(line);
}
