#include <QPainter>
#include "CORE/config.h"
#include "legenditem.h"

LegendItem::LegendItem(int index)
{
	_index = index;
	_size = 0;

	_text.setParentItem(this);
	_text.setFont(QFont(FONT_FAMILY));

	setZValue(1);
}

void LegendItem::setColor(const QColor &color)
{
	setBrush(QBrush(color));
}

void LegendItem::setText(const QString &text)
{
	_text.setText(text);
}

void LegendItem::setSize(int size)
{
	_size = size;

	if (!size) {
		setVisible(false);
	} else {
		qreal width = _size * LEGEND_RECT_RATIO;
		prepareGeometryChange();
		setRect(0, 0, width, _size);
		setPos(LEGEND_MARGIN, LEGEND_MARGIN + _index * width);

		QFont font = _text.font();
		font.setPixelSize(size);
		_text.setFont(font);
		_text.setPos(width + (_size / 3), 0);

		if (!isVisible())
			setVisible(true);
	}
}

void LegendItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
  QWidget *widget = 0)
{
	bool antialiasing = painter->testRenderHint(QPainter::Antialiasing);

	if (antialiasing) {
		painter->setRenderHint(QPainter::Antialiasing, false);
		QGraphicsRectItem::paint(painter, option, widget);
		painter->setRenderHint(QPainter::Antialiasing, true);
	} else
		QGraphicsRectItem::paint(painter, option, widget);
}
