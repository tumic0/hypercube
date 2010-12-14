#ifndef GRAPHVIEW_H_
#define GRAPHVIEW_H_

#include <QGraphicsView>

class QGraphicsScene;
class EdgeItem;
class VertexItem;


class GraphView : public QGraphicsView
{
	Q_OBJECT

public:
	GraphView(QWidget *parent = 0);
	virtual ~GraphView();

	int graphSize();
	void setDimensions(const QPoint dimensions);
	QPoint dimensions(void) const;

	VertexItem* vertex(int id);
	VertexItem* addVertex();
	EdgeItem* edge(int src, int dst);
	EdgeItem* addEdge(int src, int dst);
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

	QGraphicsRectItem *_boundingRect;
	QList<VertexItem*> _vertexList;
	QList<EdgeItem*> _edgeList;

	int _graphSize;
	QPoint _dimensions;
};

#endif /* GRAPHSCENE_H_ */
