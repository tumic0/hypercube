#ifndef EDGEITEM_H_
#define EDGEITEM_H_

#include <QGraphicsLineItem>

class VertexItem;
class GraphView;

class EdgeItem : public QGraphicsLineItem
{
public:
	EdgeItem(VertexItem *src, VertexItem *dest);
	virtual ~EdgeItem();

	VertexItem* srcVertex(void) {return _src;}
	VertexItem* destVertex(void) {return _dest;}
	void adjust();

	QColor color(void) const {return _color;}
	void setColor(const QColor &color);
	qreal size();
	void setSize(qreal size);
	QString text() const {return _text->text();}
	void setText(const QString &text) {_text->setText(text);}
	int fontSize() {return _text->font().pixelSize();}
	void setFontSize(int size);

private:
	QLineF edgeLine();
	QPointF textPos();

	VertexItem *_src, *_dest;
	QPointF _srcPoint;
	QPointF _destPoint;

	QGraphicsSimpleTextItem* _text;

	int _value;
	qreal _size;
	QColor _color;
};

#endif /* EDGEITEM_H_ */
