#ifndef EDGEITEM_H_
#define EDGEITEM_H_

#include <QGraphicsItem>

class VertexItem;
class GraphView;

class EdgeItem : public QGraphicsItem
{
public:
	EdgeItem(VertexItem *src, VertexItem *dest);

	VertexItem* src() const {return _src;}
	VertexItem* dst() const {return _dst;}
	void adjust();

	bool twin() const {return _twin;}
	void setTwin(bool state) {_twin = state;}
	bool directed() const {return _directed;}
	void setDirected(bool val);
	QColor color() const {return _color;}
	void setColor(const QColor &color);
	qreal size() const {return _size;}
	void setSize(qreal size);
	QString text() const {return _text.text();}
	void setText(const QString &text) {_text.setText(text);}
	int fontSize() const {return _fontSize;}
	void setFontSize(int size);

protected:
	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
	  QWidget *widget);

private:
	VertexItem *_src, *_dst;

	QLineF edgeLine();
	QPointF textPos();

	QGraphicsSimpleTextItem _text;

	bool _directed;
	bool _twin;
	qreal _size;
	QColor _color;
	int _fontSize;
};

#endif /* EDGEITEM_H_ */
