#include <QPainter>
#include "edgeitem.h"
#include "vertexitem.h"
#include "CORE/config.h"


/*!
	\class EdgeItem
	\brief Defines edge items that can be added to a GraphView.

	Lorem ipsun...
*/

/*!
	Constructs an edge from vertex \a src to vertex \a dst.
*/
EdgeItem::EdgeItem(VertexItem *src, VertexItem *dst)
{
	_color = QColor();
	_size = 0;

	_text = new QGraphicsSimpleTextItem(QString::null, this);
	_text->setFont(QFont(FONT_FAMILY));

	setAcceptedMouseButtons(0);
	setZValue(-1);

	_src = src;
	_dest = dst;
	_src->addEdge(this);
	_dest->addEdge(this);
	adjust();
}

EdgeItem::~EdgeItem()
{
	delete _text;
}

void EdgeItem::adjust()
{
	setLine(edgeLine());
	_text->setPos(textPos());
}

qreal EdgeItem::size()
{
	return _size;
}

void EdgeItem::setSize(qreal size)
{
	_size = size;

	if (!_size) {
		setVisible(false);
	} else {
		if (!isVisible())
			setVisible(true);
		setPen(QPen(QBrush(_color), _size));
		_text->setPos(textPos());
	}
}

void EdgeItem::setColor(const QColor &color)
{
	_color = color;
	setPen(QPen(QBrush(_color), _size));
	_text->setBrush(QBrush(color));
}

void EdgeItem::setFontSize(int size)
{
	if (size <= 0) {
		_text->setVisible(false);
	} else {
		if (!_text->isVisible())
			_text->setVisible(true);
		QFont font = _text->font();
		font.setPixelSize(size);
		_text->setFont(font);
		_text->setPos(textPos());
	}

	update(boundingRect());
}

QLineF EdgeItem::edgeLine()
{
	if (!_src || !_dest)
		return QLineF();

	QPointF sourceOffset = QPointF(_src->size() / 2, _src->size() / 2);
	QPointF destOffset = QPointF(_dest->size() / 2, _dest->size() / 2);

	QLineF line(mapFromItem(_src, 0, 0) + sourceOffset,
	  mapFromItem(_dest, 0, 0) + destOffset);

	return line;
}

QPointF EdgeItem::textPos()
{
	QLineF line = edgeLine();

	if ((line.dx() > 0 && line.dy() > 0) || (line.dx() < 0 && line.dy() < 0))
		return (line.pointAt(0.5) + QPointF(_size / 2, - _size / 2)
		  - QPointF(0, _text->font().pixelSize()));
	else
		return (line.pointAt(0.5) + QPointF(_size / 2, _size / 2));
}
