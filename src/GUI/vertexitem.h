#ifndef VERTEXITEM_H_
#define VERTEXITEM_H_

#include <QGraphicsEllipseItem>
#include <QList>

class EdgeItem;
class QGraphicsSceneMouseEvent;

class VertexItem : public QGraphicsEllipseItem
{
public:
	VertexItem(int id);

	int id() const {return _id;}

	void addEdge(EdgeItem *edge);
	QList<EdgeItem *> edges() const {return _edgeList;}

	QPointF coordinates() const {return _coordinates;}
	void setCoordinates(const QPointF &coordinates) {_coordinates = coordinates;}
	QColor color() const {return _color;}
	void setColor(const QColor &color);
	qreal size() const {return _size;}
	void setSize(qreal size);
	QString text() const {return _text.text();}
	void setText(const QString &text) {_text.setText(text);}
	int fontSize() const {return _fontSize;}
	void setFontSize(int size);

	QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);


	int _id;
	QPointF _coordinates;
	qreal _size;
	QColor _color;
	int _fontSize;

	QGraphicsSimpleTextItem _text;

	QList<EdgeItem *> _edgeList;
};

#endif /* VERTEXITEM_H_ */
