#ifndef GRAPHTAB_H_
#define GRAPHTAB_H_

#include "graphview.h"
#include "CORE/graph.h"
#include "CORE/sa.h"
#include "IO/io.h"


class GraphTab : public QWidget
{
	Q_OBJECT

public:
	GraphTab();
	virtual ~GraphTab();

	bool enabled() {return _enabled;}
	void setEnabled(bool state);

	IO::Error readGraph(const QString &fileName);
	IO::Error readGraph();
	IO::Error writeGraph(const QString &fileName, OutputProvider *provider);
	IO::Error writeGraph();
	void transformGraph();
	void project(Graph *graph);

	Graph *graph();
	GraphView *view() {return _view;}

	QPoint dimensions() const {return _dimensions;}
	void setDimensions(const QPoint &dimensions);
	QColor vertexColor() const {return _vertexColor;}
	void setVertexColor(const QColor &color);
	QColor edgeColor() const {return _edgeColor;}
	void setEdgeColor(const QColor &color);
	int vertexSize() {return _vertexSize;}
	void setVertexSize(int size);
	int edgeSize() {return _edgeSize;}
	void setEdgeSize(int size);
	int vertexFontSize() {return _vertexFontSize;}
	int edgeFontSize() {return _edgeFontSize;}
	void setVertexFontSize(int size);
	void setEdgeFontSize(int size);
	bool vertexIDs() {return _showVertexIDs;}
	bool edgeValues() {return _showEdgeValues;}
	void showVertexIDs(bool show);
	void showEdgeValues(bool show);
	void setEdgeZValue(int value);

	float nodeDistribution() {return _sa->nodeDistribution();}
	void setNodeDistribution(float value) {_sa->setNodeDistribution(value);}
	float edgeLength() {return _sa->edgeLength();}
	void setEdgeLength(float value) {_sa->setEdgeLength(value);}
	float edgeCrossings() {return _sa->edgeCrossings();}
	void setEdgeCrossings(float value) {_sa->setEdgeCrossings(value);}
	float initTemp() {return _sa->initTemp();}
	void setInitTemp(float value) {_sa->setInitTemp(value);}
	float finalTemp() {return _sa->finalTemp();}
	void setFinalTemp(float value) {_sa->setFinalTemp(value);}
	float coolFactor() {return _sa->coolFactor();}
	void setCoolFactor(float value) {_sa->setCoolFactor(value);}
	int numSteps() {return _sa->numSteps();}
	void setNumSteps(int value) {_sa->setNumSteps(value);}
#ifdef SA_LOG_SUPPORT
	bool logInfo() {return _sa->logInfo();}
	void setLogInfo(bool value) {_sa->setLogInfo(value);}
#endif

	QString& fileName() {return _inputFileName;}

private:
	void loadGraph();
	void storeGraph();
	void setGraphProperties();

	bool _enabled;

	Graph *_graph;
	GraphView *_view;
	SA *_sa;

	QString _inputFileName;
	QString _outputFileName;
	InputProvider *_inputProvider;
	OutputProvider *_outputProvider;

	QPoint _dimensions;
	QColor _vertexColor;
	QColor _edgeColor;
	int _vertexSize;
	int _edgeSize;
	int _vertexFontSize;
	int _edgeFontSize;
	bool _showVertexIDs;
	bool _showEdgeValues;
};

#endif /* GRAPHTAB_H_ */
