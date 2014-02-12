#ifndef LEGENDITEM_H_
#define LEGENDITEM_H_

#include <QGraphicsRectItem>

class LegendItem : public QGraphicsRectItem
{
public:
	LegendItem(int index);

	void setText(const QString &text);
	void setColor(const QColor &color);
	void setSize(int size);

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
	  QWidget *widget);

private:
	int _index;
	int _size;
	QGraphicsSimpleTextItem _text;
};

#endif /* LEGENDITEM_H_ */
