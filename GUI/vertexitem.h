#ifndef VERTEXITEM_H_
#define VERTEXITEM_H_

#include <QGraphicsEllipseItem>
#include <QList>

class EdgeItem;
class QGraphicsSceneMouseEvent;

class VertexItem : public QGraphicsEllipseItem
{
public:
	VertexItem();
	virtual ~VertexItem();

	void addEdge(EdgeItem *edge);
	QList<EdgeItem *> edges() const {return _edgeList;}

	QColor color() const {return _color;}
	void setColor(const QColor &color);
	qreal size() {return _size;}
	void setSize(qreal size);
	QString text() const {return _text->text();}
	void setText(const QString &text) {_text->setText(text);}
	int fontSize() {return _text->font().pixelSize();}
	void setFontSize(int size);

	QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

	QGraphicsSimpleTextItem* _text;

	QList<EdgeItem *> _edgeList;
	qreal _size;
	QColor _color;
};

#endif /* VERTEXITEM_H_ */
