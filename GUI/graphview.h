#ifndef GRAPHVIEW_H_
#define GRAPHVIEW_H_

#include <QGraphicsView>

class QGraphicsScene;
class EdgeItem;
class VertexItem;
class BoundingRectItem;


class GraphView : public QGraphicsView
{
	Q_OBJECT

public:
	GraphView(QWidget *parent = 0);
	virtual ~GraphView();

	void setDimensions(const QPoint dimensions);
	QPoint dimensions(void) const;


	VertexItem* addVertex();
	EdgeItem* addEdge(VertexItem *src, VertexItem *dst);

	int vertex_size() {return _vertexes.size();}
	int edge_size() {return _edges.size();}
	VertexItem* vertex(int id) {return _vertexes.at(id);}
	EdgeItem* edge(int id) {return _edges.at(id);}


	void clear();

	void showEdgeValues(bool visible);
	void showVertexIDs(bool visible);
	void setVertexColor(const QColor &color);
	void setEdgeColor(const QColor &color);
	void setVertexSize(int size);
	void setEdgeSize(int size);
	void setVertexFontSize(int size);
	void setEdgeFontSize(int size);
	void setEdgeZValue(int value);

	void scale(qreal scaleFactor);
	qreal zoom() {return transform().m11();}

signals:
	void zoomed(qreal z);

private:
	void wheelEvent(QWheelEvent *event);

	QGraphicsScene *_scene;

	BoundingRectItem *_boundingRect;
	QVector<VertexItem*> _vertexes;
	QVector<EdgeItem*> _edges;

	QPoint _dimensions;
};

#endif /* GRAPHSCENE_H_ */
