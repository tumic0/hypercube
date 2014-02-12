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

	const QString &attribute() const {return _attribute;}
	void setAttribute(const QString &attribute);
	void addAttribute(const QString &attribute, const QString &value);
	const QMap<QString, QString> &attributes() const {return _attributes;}

	QPoint coordinates() const {return _coordinates;}
	void setCoordinates(const QPoint &coordinates);
	QColor color() const {return _color;}
	void setColor(const QColor &color);
	qreal size() const {return _size;}
	void setSize(qreal size);
	int fontSize() const {return _fontSize;}
	void setFontSize(int size);

	QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);


	int _id;
	QPoint _coordinates;
	qreal _size;
	QColor _color;
	int _fontSize;

	QString _attribute;
	QMap<QString, QString> _attributes;
	QGraphicsSimpleTextItem _text;

	QList<EdgeItem *> _edgeList;
};

#endif /* VERTEXITEM_H_ */
