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

	VertexItem* src(void) {return _src;}
	VertexItem* dst(void) {return _dest;}
	void adjust();

	QColor color(void) const {return _color;}
	void setColor(const QColor &color);
	qreal size();
	void setSize(qreal size);
	QString text() const {return _text->text();}
	void setText(const QString &text) {_text->setText(text);}
	int fontSize() {return _fontSize;}
	void setFontSize(int size);

private:
	VertexItem *_src, *_dest;

	QLineF edgeLine();
	QPointF textPos();

	QGraphicsSimpleTextItem* _text;

	qreal _size;
	QColor _color;
	int _fontSize;
};

#endif /* EDGEITEM_H_ */
