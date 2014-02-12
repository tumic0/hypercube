#ifndef GRAPHTAB_H_
#define GRAPHTAB_H_

#include <QWidget>
#include <QSet>
#include "graphview.h"
#include "CORE/graph.h"
#include "CORE/sa.h"
#include "IO/io.h"
#include "IO/encoding.h"


class GraphTab : public QWidget
{
	Q_OBJECT

public:
	GraphTab();
	virtual ~GraphTab();

	IO::Error readGraph(const QString &fileName);
	IO::Error readGraph();
	IO::Error writeGraph(const QString &fileName, OutputProvider *provider);
	IO::Error writeGraph();

	void transformGraph();
	void bindTo(Graph *graph);
	void project(Graph *graph);
	void colorizeEdges();

	Graph *graph();
	GraphView *view() const {return _view;}

	Encoding *inputEncoding() const {return _inputEncoding;}
	void setInputEncoding(Encoding *encoding) {_inputEncoding = encoding;}
	bool antialiasing() const;
	void setAntialiasing(bool value);

	const QString &nodeLabelAttr() const {return _nodeLabelAttr;}
	void setNodeLabelAttr(const QString &name);
	const QString &edgeLabelAttr() const {return _edgeLabelAttr;}
	void setEdgeLabelAttr(const QString &name);
	const QSet<QString> &nodeLabelAttributes() const
	  {return _nodeLabelAttributes;}
	const QSet<QString> &edgeLabelAttributes() const
	  {return _edgeLabelAttributes;}

	QPoint dimensions() const {return _dimensions;}
	void setDimensions(const QPoint &dimensions);
	QColor vertexColor() const {return _vertexColor;}
	void setVertexColor(const QColor &color);
	QColor edgeColor() const {return _edgeColor;}
	void setEdgeColor(const QColor &color);
	int vertexSize() const {return _vertexSize;}
	void setVertexSize(int size);
	int edgeSize() const {return _edgeSize;}
	void setEdgeSize(int size);
	int vertexFontSize() const {return _vertexFontSize;}
	int edgeFontSize() const {return _edgeFontSize;}
	void setVertexFontSize(int size);
	void setEdgeFontSize(int size);
	bool vertexIDs() const {return _showVertexIDs;}
	bool edgeValues() const {return _showEdgeValues;}
	bool coloredEdges() const {return _coloredEdges;}
	void presetColoredEdges() {_coloredEdges = true;}
	int legend() const {return _legend;}
	bool directedGraph() const;
	void setDirectedGraph(bool state);
	void showVertexIDs(bool show);
	void showEdgeValues(bool show);
	void setEdgeZValue(int value);
	void setLegend(int size);


	int size() const {return _size;}
	void setSize(int value) {_size = value;}
	int planarity() const {return _planarity;}
	void setPlanarity(int value) {_planarity = value;}
	int quality() const {return _quality;}
	void setQuality(int value) {_quality = value;}

	float nodeDistribution() const {return _sa->nodeDistribution();}
	void setNodeDistribution(float value) {_sa->setNodeDistribution(value);}
	float edgeLength() const {return _sa->edgeLength();}
	void setEdgeLength(float value) {_sa->setEdgeLength(value);}
	float edgeCrossings() const {return _sa->edgeCrossings();}
	void setEdgeCrossings(float value) {_sa->setEdgeCrossings(value);}
	float initTemp() const {return _sa->initTemp();}
	void setInitTemp(float value) {_sa->setInitTemp(value);}
	float finalTemp() const {return _sa->finalTemp();}
	void setFinalTemp(float value) {_sa->setFinalTemp(value);}
	float coolFactor() const {return _sa->coolFactor();}
	void setCoolFactor(float value) {_sa->setCoolFactor(value);}
	int numSteps() const {return _sa->numSteps();}
	void setNumSteps(int value) {_sa->setNumSteps(value);}
#ifdef SA_LOG_SUPPORT
	bool logInfo() const {return _sa->logInfo();}
	void setLogInfo(bool value) {_sa->setLogInfo(value);}
#endif

	const QString& fileName() const {return _inputFileName;}

private:
	void loadGraph();
	void storeGraph();
	void getGraphProperties();
	void setGraphProperties();
	void updateLegend();
	void updateCoordinates();
	void updateColors();

	Graph *_graph;
	GraphView *_view;
	SA *_sa;

	QString _inputFileName;
	QString _outputFileName;
	InputProvider *_inputProvider;
	OutputProvider *_outputProvider;

	Encoding *_inputEncoding;

	QString _nodeLabelAttr;
	QString _edgeLabelAttr;
	QSet<QString> _nodeLabelAttributes;
	QSet<QString> _edgeLabelAttributes;

	QPoint _dimensions;
	QColor _vertexColor;
	QColor _edgeColor;
	int _vertexSize;
	int _edgeSize;
	int _vertexFontSize;
	int _edgeFontSize;
	int _legend;
	bool _showVertexIDs;
	bool _showEdgeValues;
	bool _coloredEdges;

	int _size;
	int _planarity;
	int _quality;
};

#endif /* GRAPHTAB_H_ */
