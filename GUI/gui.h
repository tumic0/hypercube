#ifndef GUI_H_
#define GUI_H_

#include <QtGui>

class GraphTab;
class FloatEdit;
class IntEdit;
class ColorComboBox;


class GUI : public QMainWindow
{
	Q_OBJECT

public:
	GUI();

private slots:
	void about();
	void saveFile();
	void saveAs();
	void saveAll();
	void openFile();
	void closeFile();
	void transformGraph();
	void reloadGraph();
	void bindGraph();
	void projectGraph();

	void setNodeDistribution(float value);
	void setEdgeLength(float value);
	void setEdgeCrossings(float value);
	void setInitTemp(float value);
	void setFinalTemp(float value);
	void setCoolFactor(float value);
	void setNumSteps(int value);
#ifdef SA_LOG_SUPPORT
	void setSALogInfo(int state);
#endif

	void setGraphWidth(int width);
	void setGraphHeight(int height);

	void setVertexColor(const QColor &color);
	void setEdgeColor(const QColor &color);
	void setVertexSize(int size);
	void setEdgeSize(int size);
	void setEdgeFontSize(int size);
	void setVertexFontSize(int size);
	void colorizeEdges(int state);

	void showEdgeValues(int state);
	void showVertexIDs(int state);
	void directGraph(int state);

	void setInputEncoding(int index);
	void setAntialiasing(int state);
	void setSpecialsEscaping();

	void tabChanged(int current);
	void zoom(qreal zoom);

private:
	void createActions();
	void createMenus();
	void createToolBars();
	void createStatusBar();

	void createViews();
	void createProperties();
	void createSAProperties();
	void createGraphProperties();
	void createMiscProperties();

	void getMiscProperties(GraphTab *tab);
	void getSAProperties(GraphTab *tab);
	void getGraphProperties(GraphTab *tab);
	void setSAProperties(GraphTab *tab);
	void setGraphProperties(GraphTab *tab);
	void setMiscProperties(GraphTab *tab);

	void getArguments();
	void closeEvent(QCloseEvent *event);
	void readSettings();
	void writeSettings();


	QMenu *_fileMenu;
	QMenu *_aboutMenu;
	QMenu *_graphMenu;
	QMenu *_projectionsMenu;

	QToolBar *_fileToolBar;
	QToolBar *_graphToolBar;
	QToolBar *_projectionsToolBar;

	QWidget *_graphProperties;
	QWidget *_SAProperties;
	QWidget *_MiscProperties;

	QActionGroup *_fileActionGroup;
	QActionGroup *_graphActionGroup;
	QActionGroup *_projectActionGroup;

	QAction *_exitAction;
	QAction *_aboutAction;
	QAction *_aboutQtAction;
	QAction *_saveFileAction;
	QAction *_saveAsAction;
	QAction *_saveAllAction;
	QAction *_openFileAction;
	QAction *_closeFileAction;
	QAction *_transformAction;
	QAction *_reloadAction;
	QAction *_projectAction;
	QAction *_bindAction;

	QToolBox *_properties;
	QTabWidget *_viewTab;

	QSpinBox *_graphWidth;
	QSpinBox *_graphHeight;
	QSpinBox *_edgeSize;
	QSpinBox *_vertexSize;
	QSpinBox *_edgeFontSize;
	QSpinBox *_vertexFontSize;
	QCheckBox *_edgeValues;
	QCheckBox *_vertexIDs;
	QCheckBox *_coloredEdges;
	QCheckBox *_directedGraph;
	ColorComboBox *_edgeColor;
	ColorComboBox *_vertexColor;

	FloatEdit *_nodeDistribution;
	FloatEdit *_edgeLength;
	FloatEdit *_edgeCrossings;
	FloatEdit *_initTemp;
	FloatEdit *_finalTemp;
	FloatEdit *_coolFactor;
	IntEdit *_numSteps;
	QCheckBox *_debug;

	QComboBox *_inputEncoding;
	QCheckBox *_antialiasing;
	QLabel *_arguments;
	QCheckBox *_argumentsEscape;

	QLabel *_fileName;
	QLabel *_zoom;
};

#endif // GUI_H_
