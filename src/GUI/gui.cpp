#include <QApplication>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMenuBar>
#include <QStatusBar>
#include <QGroupBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QCloseEvent>
#include <cmath>
#include "gui.h"
#include "colorcombobox.h"
#include "numericedit.h"
#include "graphtab.h"
#include "icons.h"
#include "settings.h"
#include "CORE/config.h"
#include "IO/io.h"
#include "IO/encoding.h"
#include "IO/modules.h"


#define EDGE_LENGTH_ORDER_RANGE       1
#define NODE_DISTRIBUTION_ORDER_RANGE 1
#define EDGE_CROSSINGS_ORDER_RANGE    2
#define NUM_STEPS_ORDER_RANGE         0.5
#define INIT_TEMP_ORDER_RANGE         0.5


#define TAB() \
	((GraphTab*) _viewTab->currentWidget())

#define BLOCK(widget, action) \
	widget->blockSignals(true); \
	widget->action; \
	widget->blockSignals(false)

#define CHECKED(state) \
	((state == Qt::Checked) ? true : false)

#define ZOOM_STRING(zoom) \
	QString("%1%").arg((int)((zoom) * 100))

#define MIN_VAL(midpoint, orderRange) \
	(pow(10.0f, log10((float)(midpoint)) - (float)(orderRange)))
#define MAX_VAL(midpoint, orderRange) \
	(pow(10.0f, log10((float)(midpoint)) + (float)(orderRange)))
#define SCALE(min, max) \
	(((max) - (min)) / 100.0f)


static void initComboBox(QComboBox *cb, const QSet<QString> &set)
{
	QList<QString> list;
	QList<QString>::const_iterator it;

	list = set.values();
	qSort(list);

	cb->blockSignals(true);
	cb->clear();
	for (it = list.begin(); it != list.end(); it++)
		cb->addItem(*it);
	cb->blockSignals(false);

	cb->setDisabled(cb->count() > 1 ? false : true);
}

static void setComboBox(QComboBox *cb, const QString &value)
{
	for (int i = 0; i < cb->count(); i++) {
		if (cb->itemText(i) == value) {
			BLOCK(cb, setCurrentIndex(i));
			break;
		}
	}
}

static QString saveFilter(OutputProvider *provider)
{
	return QString(provider->description()) + QString(" (*.")
	  + QString(provider->type()) + QString(")");
}

static QString errorDescription(IO::Error error)
{
	switch(error) {
		case IO::OpenError:
			return  QObject::tr("Error opening file");
		case IO::ReadError:
			return QObject::tr("Error reading file");
		case IO::WriteError:
			return QObject::tr("Error writing file");
		case IO::FormatError:
			return QObject::tr("File format error");
		default:
			return QObject::tr("Unknown error");
	}
}


GUI::GUI()
{
	createActions();
	createMenus();
	createToolBars();
	createViews();
	createProperties();
	createStatusBar();

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(_properties);
	layout->addWidget(_viewTab);

	QWidget *widget = new QWidget;
	widget->setLayout(layout);
	setCentralWidget(widget);

	setWindowTitle("Hypercube");
	setUnifiedTitleAndToolBarOnMac(true);

	readSettings();
}

void GUI::createActions()
{
	// Action Groups
	_fileActionGroup = new QActionGroup(this);
	_fileActionGroup->setExclusive(false);
	_fileActionGroup->setEnabled(false);
	_graphActionGroup = new QActionGroup(this);
	_graphActionGroup->setExclusive(false);
	_graphActionGroup->setEnabled(false);
	_projectActionGroup = new QActionGroup(this);
	_projectActionGroup->setExclusive(false);
	_projectActionGroup->setEnabled(false);


	// General actions
	_exitAction = new QAction(tr("Exit"), this);
	connect(_exitAction, SIGNAL(triggered()), this, SLOT(close()));

	_aboutAction = new QAction(QIcon(QPixmap(APP_ICON)),
	  tr("About Hypercube"), this);
	connect(_aboutAction, SIGNAL(triggered()), this, SLOT(about()));
	_aboutQtAction = new QAction(tr("About Qt"), this);
	connect(_aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	// File related actions
	_openFileAction = new QAction(QIcon(QPixmap(OPEN_FILE_ICON)),
	  tr("Open file"), this);
	connect(_openFileAction, SIGNAL(triggered()), this, SLOT(openFile()));
	_saveFileAction = new QAction(QIcon(QPixmap(SAVE_FILE_ICON)),
	  tr("Save file"), this);
	_saveFileAction->setActionGroup(_fileActionGroup);
	connect(_saveFileAction, SIGNAL(triggered()), this, SLOT(saveFile()));
	_saveAsAction = new QAction(QIcon(QPixmap(SAVE_AS_ICON)),
	  tr("Save as"), this);
	_saveAsAction->setActionGroup(_fileActionGroup);
	connect(_saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));
	_saveAllAction = new QAction(QIcon(QPixmap(SAVE_ALL_ICON)),
	  tr("Save all"), this);
	_saveAllAction->setActionGroup(_fileActionGroup);
	connect(_saveAllAction, SIGNAL(triggered()), this, SLOT(saveAll()));
	_closeFileAction = new QAction(QIcon(QPixmap(CLOSE_FILE_ICON)),
	  tr("Close file"), this);
	_closeFileAction->setActionGroup(_fileActionGroup);
	connect(_closeFileAction, SIGNAL(triggered()), this, SLOT(closeFile()));

	// Graph related actions
	_transformAction = new QAction(QIcon(QPixmap(TRANSFORM_GRAPH_ICON)),
	  tr("Transform graph"), this);
	_transformAction->setActionGroup(_graphActionGroup);
	connect(_transformAction, SIGNAL(triggered()), this,
	  SLOT(transformGraph()));
	_reloadAction = new QAction(QIcon(QPixmap(RELOAD_GRAPH_ICON)),
	  tr("Reload graph"), this);
	_reloadAction->setActionGroup(_graphActionGroup);
	connect(_reloadAction, SIGNAL(triggered()), this, SLOT(reloadGraph()));

	// Projections related actions
	_projectAction = new QAction(QIcon(QPixmap(PROJECT_ICON)),
	  tr("Project appearance"), this);
	_projectAction->setActionGroup(_projectActionGroup);
	connect(_projectAction, SIGNAL(triggered()), this,
	  SLOT(projectGraph()));
	_bindAction = new QAction(QIcon(QPixmap(BIND_ICON)),
	  tr("Project layout"), this);
	_bindAction->setActionGroup(_projectActionGroup);
	connect(_bindAction, SIGNAL(triggered()), this,
	  SLOT(bindGraph()));
}

void GUI::createMenus()
{
	_fileMenu = menuBar()->addMenu(tr("File"));
	_fileMenu->addAction(_openFileAction);
	_fileMenu->addAction(_saveFileAction);
	_fileMenu->addAction(_saveAsAction);
	_fileMenu->addAction(_saveAllAction);
	_fileMenu->addAction(_closeFileAction);
	_fileMenu->addAction(_exitAction);

	_graphMenu = menuBar()->addMenu(tr("Graph"));
	_graphMenu->addAction(_transformAction);
	_graphMenu->addAction(_reloadAction);

	_projectionsMenu = menuBar()->addMenu(tr("Projections"));
	_projectionsMenu->addAction(_bindAction);
	_projectionsMenu->addAction(_projectAction);

	_aboutMenu = menuBar()->addMenu(tr("Help"));
	_aboutMenu->addAction(_aboutAction);
	_aboutMenu->addAction(_aboutQtAction);
}

void GUI::createToolBars()
{
	_fileToolBar = addToolBar(tr("File"));
	_fileToolBar->addAction(_openFileAction);
	_fileToolBar->addAction(_saveFileAction);
	_fileToolBar->addAction(_saveAllAction);
	_fileToolBar->addAction(_closeFileAction);

	_graphToolBar = addToolBar(tr("Graph"));
	_graphToolBar->addAction(_transformAction);
	_graphToolBar->addAction(_reloadAction);

	_projectionsToolBar = addToolBar(tr("Projections"));
	_projectionsToolBar->addAction(_bindAction);
	_projectionsToolBar->addAction(_projectAction);

#ifdef Q_OS_MAC
	_fileToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	_graphToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	_projectionsToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
#endif
}

void GUI::createProperties()
{
	createGraphProperties();
	createAlgorithmProperties();
	createMiscProperties();

	_properties = new QToolBox;

	_properties->setBackgroundRole(QPalette::Window);
	_properties->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,
	  QSizePolicy::Ignored));
	_properties->setMinimumWidth(qMax(_miscProperties->sizeHint().width(),
	  qMax(_graphProperties->sizeHint().width(),
	  _algorithmProperties->sizeHint().width())));

	_properties->addItem(_graphProperties, tr("Graph settings"));
	_properties->addItem(_algorithmProperties, tr("Algorithm settings"));
	_properties->addItem(_miscProperties, tr("Miscellaneous"));
}

void GUI::createMiscProperties()
{
	QGroupBox *inputBox = new QGroupBox(tr("Input"));

	_inputEncoding = new QComboBox();

	for (Encoding **ep = encodings; *ep; ep++)
		_inputEncoding->addItem((*ep)->name());

	connect(_inputEncoding, SIGNAL(currentIndexChanged(int)), this,
	  SLOT(setInputEncoding(int)));

	QVBoxLayout *inputLayout = new QVBoxLayout;
	QFormLayout *inputFormLayout = new QFormLayout;
	inputFormLayout->addRow(tr("Encoding:"), _inputEncoding);
	inputLayout->addLayout(inputFormLayout);
	inputLayout->addStretch();
	inputBox->setLayout(inputLayout);


	QGroupBox *displayBox = new QGroupBox(tr("Display"));

	_antialiasing = new QCheckBox(tr("Use antialiasing"), this);
	QVBoxLayout *displayLayout = new QVBoxLayout;
	displayLayout->addWidget(_antialiasing);
	displayLayout->addStretch();
	displayBox->setLayout(displayLayout);

	connect(_antialiasing, SIGNAL(stateChanged(int)),
	  this, SLOT(setAntialiasing(int)));


	QGroupBox *argumentsBox = new QGroupBox(tr("CLI arguments"));

	_arguments = new QLabel(this);
	_arguments->setTextInteractionFlags(Qt::TextSelectableByMouse
	  | Qt::TextSelectableByKeyboard);
	_arguments->setWordWrap(true);

	_argumentsEscape = new QCheckBox(tr("Escape special characters"));

	QVBoxLayout *argumentsLayout = new QVBoxLayout;
	argumentsLayout->addWidget(_argumentsEscape);
	argumentsLayout->addWidget(_arguments);
	argumentsLayout->addStretch();
	argumentsBox->setLayout(argumentsLayout);

	connect(_argumentsEscape, SIGNAL(stateChanged(int)),
	  this, SLOT(setSpecialsEscaping()));


	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(inputBox);
	layout->addWidget(displayBox);
	layout->addWidget(argumentsBox);

	_miscProperties = new QWidget;
	_miscProperties->setLayout(layout);
}

void GUI::createAlgorithmProperties()
{
	QComboBox *optionsType = new QComboBox();
	optionsType->addItem(tr("Basic settings"));
	optionsType->addItem(tr("Advanced settings"));

	connect(optionsType, SIGNAL(currentIndexChanged(int)),
	  this, SLOT(layoutOptionsTypeChanged(int)));


	_layoutBox = new QGroupBox(tr("Graph layout"));

	_sizeSlider = new QSlider(Qt::Horizontal);
	_sizeSlider->setRange(0, 100);
	_sizeSlider->setValue(50);
	_planaritySlider = new QSlider(Qt::Horizontal);
	_planaritySlider->setRange(0, 100);
	_planaritySlider->setValue(50);

	_compBox = new QGroupBox(tr("Computation"));

	_qualitySlider = new QSlider(Qt::Horizontal);
	_qualitySlider->setRange(0, 100);
	_qualitySlider->setValue(50);


	connect(_sizeSlider, SIGNAL(valueChanged(int)),
	  this, SLOT(setSize(int)));
	connect(_planaritySlider, SIGNAL(valueChanged(int)),
	  this, SLOT(setPlanarity(int)));
	connect(_qualitySlider, SIGNAL(valueChanged(int)),
	  this, SLOT(setQuality(int)));

	QFormLayout *layoutLayout = new QFormLayout;
	layoutLayout->addRow(tr("Size:"), _sizeSlider);
	layoutLayout->addRow(tr("Planarity:"), _planaritySlider);
	_layoutBox->setLayout(layoutLayout);

	QFormLayout *compLayout = new QFormLayout;
	compLayout->addRow(tr("Quality:"), _qualitySlider);
	_compBox->setLayout(compLayout);

	_graphBox = new QGroupBox(tr("Graph layout"));

	_nodeDistribution = new FloatEdit(10);
	_edgeLength = new FloatEdit(10);
	_edgeCrossings = new FloatEdit(10);

	connect(_nodeDistribution, SIGNAL(valueChanged(float)),
	  this, SLOT(setNodeDistribution(float)));
	connect(_edgeLength, SIGNAL(valueChanged(float)),
	  this, SLOT(setEdgeLength(float)));
	connect(_edgeCrossings, SIGNAL(valueChanged(float)),
	  this, SLOT(setEdgeCrossings(float)));

	QFormLayout *graphLayout = new QFormLayout;
	graphLayout->addRow(tr("Node distribution:"), _nodeDistribution);
	graphLayout->addRow(tr("Edge length:"), _edgeLength);
	graphLayout->addRow(tr("Edge crossings:"), _edgeCrossings);
	_graphBox->setLayout(graphLayout);


	_SABox = new QGroupBox(tr("Computation"));

	_initTemp = new FloatEdit(10);
	_finalTemp = new FloatEdit(10);
	_coolFactor = new FloatEdit(10);
	_numSteps = new IntEdit(10);

	connect(_initTemp, SIGNAL(valueChanged(float)),
	  this, SLOT(setInitTemp(float)));
	connect(_finalTemp, SIGNAL(valueChanged(float)),
	  this, SLOT(setFinalTemp(float)));
	connect(_coolFactor, SIGNAL(valueChanged(float)),
	  this, SLOT(setCoolFactor(float)));
	connect(_numSteps, SIGNAL(valueChanged(int)),
	  this, SLOT(setNumSteps(int)));

	QFormLayout *SAlayout = new QFormLayout;
	SAlayout->addRow(tr("Initial temperature:"), _initTemp);
	SAlayout->addRow(tr("Final temperature:"), _finalTemp);
	SAlayout->addRow(tr("Cooling factor:"), _coolFactor);
	SAlayout->addRow(tr("Steps:"), _numSteps);
	_SABox->setLayout(SAlayout);

#ifdef SA_LOG_SUPPORT
	_debugBox = new QGroupBox(tr("Debug"));
	_debug = new QCheckBox(tr("Create debug output"), this);
	QVBoxLayout *debugLayout = new QVBoxLayout;
	debugLayout->addWidget(_debug, 0, Qt::AlignTop);
	_debugBox->setLayout(debugLayout);

	connect(_debug, SIGNAL(stateChanged(int)),
	  this, SLOT(setSALogInfo(int)));
#endif


	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(_layoutBox);
	layout->addWidget(_compBox);
	layout->addWidget(_graphBox);
	layout->addWidget(_SABox);
#ifdef SA_LOG_SUPPORT
	layout->addWidget(_debugBox);
#endif
	layout->addWidget(optionsType);

	_algorithmProperties = new QWidget;
	_algorithmProperties->setLayout(layout);

	layoutOptionsTypeChanged(0);
}

void GUI::createGraphProperties()
{
	_directedGraph = new QCheckBox(tr("Directed graph"), this);

	_graphWidth = new QSpinBox();
	_graphHeight = new QSpinBox();
	_graphWidth->setMaximum(10000);
	_graphHeight->setMaximum(10000);

	connect(_directedGraph, SIGNAL(stateChanged(int)),
	  this, SLOT(directGraph(int)));

	connect(_graphWidth, SIGNAL(valueChanged(int)),
	  this, SLOT(setGraphWidth(int)));
	connect(_graphHeight, SIGNAL(valueChanged(int)),
	  this, SLOT(setGraphHeight(int)));

	QGroupBox *graphBox = new QGroupBox(tr("Graph"));

	QFormLayout *graphLayout = new QFormLayout;
	graphLayout->addRow(tr("Width:"), _graphWidth);
	graphLayout->addRow(tr("Height:"), _graphHeight);
	graphLayout->addRow(_directedGraph);
	graphBox->setLayout(graphLayout);


	_vertexSize = new QSpinBox();
	_edgeSize = new QSpinBox();
	_edgeFontSize = new QSpinBox();
	_vertexFontSize = new QSpinBox();
	_edgeColor = new ColorComboBox();
	_vertexColor = new ColorComboBox();
	_vertexIDs = new QCheckBox(tr("Show labels"), this);
	_edgeValues = new QCheckBox(tr("Show labels"), this);
	_coloredEdges = new QCheckBox(tr("Colored edges"), this);
	_legend = new QCheckBox(tr("Show legend"), this);
	_vertexLabelAttr = new QComboBox();
	_edgeLabelAttr = new QComboBox();

	_edgeFontSize->setMinimum(MIN_FONT_SIZE);
	_vertexFontSize->setMinimum(MIN_FONT_SIZE);
	_vertexLabelAttr->setMinimumSize(_vertexColor->sizeHint());
	_vertexLabelAttr->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	_edgeLabelAttr->setMinimumSize(_edgeColor->sizeHint());
	_edgeLabelAttr->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	_vertexLabelAttr->setDisabled(true);
	_edgeLabelAttr->setDisabled(true);

	connect(_edgeSize, SIGNAL(valueChanged(int)),
	  this, SLOT(setEdgeSize(int)));
	connect(_vertexSize, SIGNAL(valueChanged(int)),
	  this, SLOT(setVertexSize(int)));
	connect(_edgeFontSize, SIGNAL(valueChanged(int)),
	  this, SLOT(setEdgeFontSize(int)));
	connect(_vertexFontSize, SIGNAL(valueChanged(int)),
	  this, SLOT(setVertexFontSize(int)));
	connect(_coloredEdges, SIGNAL(stateChanged(int)),
	  this, SLOT(colorizeEdges(int)));
	connect(_legend, SIGNAL(stateChanged(int)),
	  this, SLOT(showLegend(int)));

	connect(_edgeColor, SIGNAL(activated(const QColor&)),
	  this, SLOT(setEdgeColor(const QColor&)));
	connect(_vertexColor, SIGNAL(activated(const QColor&)),
	  this, SLOT(setVertexColor(const QColor&)));

	connect(_edgeValues, SIGNAL(stateChanged(int)),
	  this, SLOT(showEdgeValues(int)));
	connect(_vertexIDs, SIGNAL(stateChanged(int)),
	  this, SLOT(showVertexIDs(int)));

	connect(_vertexLabelAttr, SIGNAL(currentIndexChanged(int)), this,
	  SLOT(setNodeLabelAttr(int)));
	connect(_edgeLabelAttr, SIGNAL(currentIndexChanged(int)), this,
	  SLOT(setEdgeLabelAttr(int)));

	QGroupBox *vertexBox = new QGroupBox(tr("Vertexes"));
	QGroupBox *edgeBox = new QGroupBox(tr("Edges"));

	QFormLayout *edgeLayout = new QFormLayout;
	QFormLayout *vertexLayout = new QFormLayout;

	edgeLayout->addRow(tr("Size:"), _edgeSize);
	edgeLayout->addRow(tr("Font size:"), _edgeFontSize);
	edgeLayout->addRow(tr("Color:"), _edgeColor);
	edgeLayout->addRow(tr("Label:"), _edgeLabelAttr);
	edgeLayout->addRow(_edgeValues);
	edgeLayout->addRow(_coloredEdges);
	edgeLayout->addRow(_legend);
	edgeBox->setLayout(edgeLayout);

	vertexLayout->addRow(tr("Size:"), _vertexSize);
	vertexLayout->addRow(tr("Font size:"), _vertexFontSize);
	vertexLayout->addRow(tr("Color:"), _vertexColor);
	vertexLayout->addRow(tr("Label:"), _vertexLabelAttr);
	vertexLayout->addRow(_vertexIDs);
	vertexBox->setLayout(vertexLayout);


	QVBoxLayout *layout = new QVBoxLayout;

	layout->addWidget(graphBox);
	layout->addWidget(vertexBox);
	layout->addWidget(edgeBox);

	_graphProperties = new QWidget;
	_graphProperties->setLayout(layout);
}

void GUI::createViews()
{
	_viewTab = new QTabWidget;

#if QT_VERSION >= 0x040500
	_viewTab->setTabsClosable(true);
	connect(_viewTab, SIGNAL(tabCloseRequested(int)),
	  this, SLOT(tabClosed(int)));
#endif
	connect(_viewTab, SIGNAL(currentChanged(int)),
	  this, SLOT(tabChanged(int)));
}

void GUI::createStatusBar()
{
	_fileName = new QLabel();
	_zoom = new QLabel();
	_zoom->setAlignment(Qt::AlignHCenter);

	statusBar()->addPermanentWidget(_fileName, 9);
	statusBar()->addPermanentWidget(_zoom, 1);
	statusBar()->setSizeGripEnabled(false);
}

void GUI::layoutOptionsTypeChanged(int advanced)
{
	if (advanced) {
		_layoutBox->hide();
		_compBox->hide();
		_debugBox->show();
		_SABox->show();
		_graphBox->show();
	} else {
		_layoutBox->show();
		_compBox->show();
		_debugBox->hide();
		_SABox->hide();
		_graphBox->hide();
	}
}

void GUI::checkActions()
{
	int cnt = _viewTab->count();
	if (cnt == 0) {
		_fileActionGroup->setEnabled(false);
		_graphActionGroup->setEnabled(false);
		_vertexLabelAttr->clear();
		_edgeLabelAttr->clear();
		_vertexLabelAttr->setEnabled(false);
		_edgeLabelAttr->setEnabled(false);
	} else if (cnt == 1)
		_projectActionGroup->setEnabled(false);
}

void GUI::tabClosed(int current)
{
	GraphTab* tab = (GraphTab*)(_viewTab->widget(current));

	_viewTab->removeTab(current);
	delete tab;

	checkActions();
}

void GUI::tabChanged(int current)
{
	if (current == -1) {
		_fileName->setText(QString::null);
		_zoom->setText(QString::null);
		return;
	}

	GraphTab *tab = (GraphTab*) _viewTab->widget(current);

	getAlgorithmProperties(tab);
	getGraphProperties(tab);
	getMiscProperties(tab);

	_zoom->setText(ZOOM_STRING(tab->view()->zoom()));
	_fileName->setText(tab->fileName());

	getArguments();
}

void GUI::about()
{
	QMessageBox::about(this, tr("About Hypercube"),
	  QString("<h3>") + QString("Hypercube ") + QString(APP_VERSION)
	  + QString("</h3><p>") + tr("Graph visualizing tool") + QString("<p/>")
	  + QString("<p>") + tr("Hypercube is distributed under the terms of the "
		"GNU General Public License version 3. For more info about Hypercube "
		"visit the project homepage at")
	  + QString(" <a href=\""APP_HOMEPAGE"\">"APP_HOMEPAGE"</a>.</p>"));
}

void GUI::openFile()
{
	QStringList files = QFileDialog::getOpenFileNames(this, tr("Open file"));
	QStringList list = files;
	QStringList::Iterator it = list.begin();

	while(it != list.end()) {
		openFile(*it);
		++it;
	}
}

void GUI::openFile(const QString &fileName)
{
	GraphTab *tab = new GraphTab();

	setAlgorithmProperties(tab);
	setGraphProperties(tab);
	setMiscProperties(tab);

	IO::ioerr.str("");
	IO::Error error = tab->readGraph(fileName);

	if (error) {
		std::cerr << IO::ioerr.str();
		QMessageBox::critical(this, tr("Error"),
		  fileName + QString("\n\n")
		  + tr("Error loading graph") + QString(":\n")
		  + errorDescription(error));
		delete tab;
	} else {
		getGraphProperties(tab);

		QFileInfo fi(fileName);
		int index = _viewTab->addTab(tab, fi.fileName());

		connect(tab->view(), SIGNAL(zoomed(qreal)), SLOT(zoom(qreal)));

		_viewTab->setCurrentIndex(index);

		int cnt = _viewTab->count();
		if (cnt == 1) {
			_fileActionGroup->setEnabled(true);
			_graphActionGroup->setEnabled(true);
		} else if (cnt == 2)
			_projectActionGroup->setEnabled(true);
	}
}

void GUI::saveFile()
{
	if (TAB()->writeGraph() != IO::Ok)
		emit saveAs();
}

void GUI::saveAs()
{
	QString filter,  selectedFilter;
	OutputProvider **provider = outputProviders;

	while (*provider) {
		filter += saveFilter(*provider);
		provider++;
		if (*provider)
			filter += QString(";;");
	}

	QString fileName = QFileDialog::getSaveFileName(this,
	  tr("Save file"), QString::null, filter, &selectedFilter);
	if (!fileName.isEmpty()) {
		provider = outputProviders;
		while (*provider) {
			if (selectedFilter == saveFilter(*provider))
				break;
			provider++;
		}

		QString suffix = QString('.') + QString((*provider)->type());
		if (!fileName.endsWith(suffix))
			fileName.append(suffix);

		IO::ioerr.str("");
		IO::Error error = TAB()->writeGraph(fileName, *provider);
		if (error) {
			std::cerr << IO::ioerr.str();
			QMessageBox::critical(this, tr("Error"),
			  fileName + QString("\n\n")
			  + tr("Error saving graph") + QString(":\n")
			  + errorDescription(error));
		}
	}
}

void GUI::saveAll()
{
	int index = _viewTab->currentIndex();

	for (int i = 0; i < _viewTab->count(); i++) {
		_viewTab->setCurrentIndex(i);
		saveFile();
	}

	_viewTab->setCurrentIndex(index);
}

void GUI::closeFile()
{
	GraphTab* tab = TAB();

	_viewTab->removeTab(_viewTab->indexOf(tab));
	delete tab;

	checkActions();
}

void GUI::transformGraph()
{
	TAB()->transformGraph();
}

void GUI::reloadGraph()
{
	IO::ioerr.str("");
	IO::Error error = TAB()->readGraph();
	if (error) {
		std::cerr << IO::ioerr.str();
		QMessageBox::critical(this, tr("Error"),
		  tr("Error loading graph") + QString(":\n")
		  + errorDescription(error));
		closeFile();
	} else {
		TAB()->transformGraph();
		getGraphProperties(TAB());
	}
}

void GUI::bindGraph()
{
	GraphTab* tab;

	for (int i = 0; i < _viewTab->count(); i++) {
		if (_viewTab->currentIndex() == i)
			continue;
		tab = ((GraphTab*) _viewTab->widget(i));
		tab->bindTo(TAB()->graph());
	}
}

void GUI::projectGraph()
{
	GraphTab* tab;

	for (int i = 0; i < _viewTab->count(); i++) {
		if (_viewTab->currentIndex() == i)
			continue;
		tab = ((GraphTab*) _viewTab->widget(i));
		tab->project(TAB()->graph());
	}
}


void GUI::setNodeDistribution(float value)
{
	if (TAB())
		TAB()->setNodeDistribution(value);
	getArguments();
}

void GUI::setEdgeLength(float value)
{
	if (TAB())
		TAB()->setEdgeLength(value);
	getArguments();
}

void GUI::setEdgeCrossings(float value)
{
	if (TAB())
		TAB()->setEdgeCrossings(value);
	getArguments();
}

void GUI::setInitTemp(float value)
{
	if (TAB())
		TAB()->setInitTemp(value);
	getArguments();
}

void GUI::setFinalTemp(float value)
{
	if (TAB())
		TAB()->setFinalTemp(value);
	getArguments();
}

void GUI::setCoolFactor(float value)
{
	if (TAB())
		TAB()->setCoolFactor(value);
	getArguments();
}

void GUI::setNumSteps(int value)
{
	if (TAB())
		TAB()->setNumSteps(value);
	getArguments();
}

void GUI::setSize(int value)
{
	float minvl = MIN_VAL(EDGE_LENGTH, EDGE_LENGTH_ORDER_RANGE);
	float maxvl = MAX_VAL(EDGE_LENGTH, EDGE_LENGTH_ORDER_RANGE);
	float minvn = MIN_VAL(NODE_DISTRIBUTION, NODE_DISTRIBUTION_ORDER_RANGE);
	float maxvn = MAX_VAL(NODE_DISTRIBUTION, NODE_DISTRIBUTION_ORDER_RANGE);
	float scalel = SCALE(minvl, maxvl);
	float scalen = SCALE(minvn, maxvn);

	_edgeLength->setValue(maxvl - scalel * value);
	_nodeDistribution->setValue(minvn + scalen * value);

	if (TAB())
		TAB()->setSize(_sizeSlider->value());
	getArguments();
}

void GUI::setPlanarity(int value)
{
	float minv = MIN_VAL(EDGE_CROSSINGS, EDGE_CROSSINGS_ORDER_RANGE);
	float maxv = MAX_VAL(EDGE_CROSSINGS, EDGE_CROSSINGS_ORDER_RANGE);
	float scale = SCALE(minv, maxv);

	_edgeCrossings->setValue(minv + scale * value);

	if (TAB())
		TAB()->setPlanarity(_planaritySlider->value());
	getArguments();
}

void GUI::setQuality(int value)
{
	float mins = MIN_VAL(NUM_STEPS, NUM_STEPS_ORDER_RANGE);
	float maxs = MAX_VAL(NUM_STEPS, NUM_STEPS_ORDER_RANGE);
	float mint = MIN_VAL(INIT_TEMP, INIT_TEMP_ORDER_RANGE);
	float maxt = MAX_VAL(INIT_TEMP, INIT_TEMP_ORDER_RANGE);
	float scales = SCALE(mins, maxs);
	float scalet = SCALE(mint, maxt);

	_numSteps->setValue(mins + scales * value);
	_initTemp->setValue(mint + scalet * value);

	if (TAB())
		TAB()->setQuality(_qualitySlider->value());
	getArguments();
}

#ifdef SA_LOG_SUPPORT
void GUI::setSALogInfo(int state)
{
	if (TAB())
		TAB()->setLogInfo(CHECKED(state));
}
#endif

void GUI::setGraphWidth(int width)
{
	if (TAB())
		TAB()->setDimensions(QPoint(width, TAB()->dimensions().y()));
	getArguments();
}

void GUI::setGraphHeight(int height)
{
	if (TAB())
		TAB()->setDimensions(QPoint(TAB()->dimensions().x(), height));
	getArguments();
}

void GUI::setVertexColor(const QColor &color)
{
	if (TAB())
		TAB()->setVertexColor(color);
	getArguments();
}

void GUI::setEdgeColor(const QColor &color)
{
	if (TAB())
		TAB()->setEdgeColor(color);
	getArguments();
}

void GUI::setVertexSize(int size)
{
	if (TAB())
		TAB()->setVertexSize(size);
	_edgeSize->setMaximum(size);
	getArguments();
}

void GUI::setEdgeSize(int size)
{
	if (TAB())
		TAB()->setEdgeSize(size);
	getArguments();
}

void GUI::setEdgeFontSize(int size)
{
	if (TAB()) {
		TAB()->setEdgeFontSize(size);
		if (_legend->isChecked())
			TAB()->setLegend(size);
	}
	getArguments();
}

void GUI::setVertexFontSize(int size)
{
	if (TAB())
		TAB()->setVertexFontSize(size);
	getArguments();
}

void GUI::showEdgeValues(int state)
{
	if (TAB())
		TAB()->showEdgeValues(CHECKED(state));
	getArguments();
}

void GUI::showVertexIDs(int state)
{
	if (TAB())
		TAB()->showVertexIDs(CHECKED(state));
	getArguments();
}

void GUI::directGraph(int state)
{
	if (TAB())
		TAB()->setDirectedGraph(CHECKED(state));
	getArguments();
}

void GUI::colorizeEdges(int state)
{
	if (CHECKED(state)) {
		if (TAB())
			TAB()->colorizeEdges();
		_edgeColor->setEnabled(false);
		_legend->setEnabled(true);
	} else {
		if (TAB()) {
			TAB()->setEdgeColor(_edgeColor->color());
			TAB()->setLegend(0);
		}
		_legend->setChecked(false);
		_legend->setEnabled(false);
		_edgeColor->setEnabled(true);
	}

	getArguments();
}

void GUI::showLegend(int state)
{
	if (TAB())
		TAB()->setLegend(CHECKED(state) ? _edgeFontSize->value() : 0);
	getArguments();
}

void GUI::setInputEncoding(int index)
{
	if (TAB()) {
		TAB()->setInputEncoding(*(encodings + index));
		reloadGraph();
	}
	getArguments();
}

void GUI::setNodeLabelAttr(int index)
{
	if (TAB())
		TAB()->setNodeLabelAttr(_vertexLabelAttr->itemText(index));
	getArguments();
}

void GUI::setEdgeLabelAttr(int index)
{
	if (TAB()) {
		TAB()->setEdgeLabelAttr(_edgeLabelAttr->itemText(index));
		if (TAB()->coloredEdges())
			TAB()->colorizeEdges();
	}
	getArguments();
}

void GUI::setAntialiasing(int state)
{
	if (TAB())
		TAB()->setAntialiasing(CHECKED(state));
}

void GUI::setSpecialsEscaping()
{
	getArguments();
}


void GUI::setMiscProperties(GraphTab *tab)
{
	Encoding* encoding = *(encodings + _inputEncoding->currentIndex());

	tab->setInputEncoding(encoding);
	tab->setAntialiasing(_antialiasing->isChecked());
}

void GUI::setAlgorithmProperties(GraphTab *tab)
{
	tab->setSize(_sizeSlider->value());
	tab->setPlanarity(_planaritySlider->value());
	tab->setQuality(_qualitySlider->value());

	tab->setNodeDistribution(_nodeDistribution->value());
	tab->setEdgeLength(_edgeLength->value());
	tab->setEdgeCrossings(_edgeCrossings->value());
	tab->setInitTemp(_initTemp->value());
	tab->setFinalTemp(_finalTemp->value());
	tab->setCoolFactor(_coolFactor->value());
	tab->setNumSteps(_numSteps->value());
#ifdef SA_LOG_SUPPORT
	tab->setLogInfo(_debug->isChecked());
#endif
}

void GUI::setGraphProperties(GraphTab *tab)
{
	tab->showVertexIDs(_vertexIDs->isChecked());
	tab->showEdgeValues(_edgeValues->isChecked());
	tab->setDirectedGraph(_directedGraph->isChecked());
	tab->setLegend(_legend->isChecked() ? _edgeFontSize->value() : 0);
	tab->setDimensions(QPoint(_graphWidth->value(), _graphHeight->value()));
	tab->setVertexSize(_vertexSize->value());
	tab->setVertexColor(_vertexColor->color());
	tab->setVertexFontSize(_vertexFontSize->value());
	tab->setEdgeSize(_edgeSize->value());
	tab->setEdgeFontSize(_edgeFontSize->value());
	tab->setEdgeColor(_edgeColor->color());
	if (_coloredEdges->isChecked())
		tab->presetColoredEdges();
}

void GUI::getArguments()
{
	QString args;
	int ef, vf;

	ef = _edgeValues->checkState() ? _edgeFontSize->value() : 0;
	vf = _vertexIDs->checkState() ? _vertexFontSize->value() : 0;

	if (_graphWidth->value() != GRAPH_WIDTH
	  || _graphHeight->value() != GRAPH_HEIGHT)
		args.append(QString(" -s %1,%2").arg(_graphWidth->value())
		  .arg(_graphHeight->value()));
	if (_edgeSize->value() != EDGE_SIZE)
		args.append(QString(" -es %1").arg(_edgeSize->value()));
	if (_vertexSize->value() != VERTEX_SIZE)
		args.append(QString(" -vs %1").arg(_vertexSize->value()));
	if (_edgeColor->isEnabled() && _edgeColor->color() != EDGE_COLOR)
		args.append(QString(" -ec %1").arg(_edgeColor->color().name()));
	if (_vertexColor->color() != VERTEX_COLOR)
		args.append(QString(" -vc %1").arg(_vertexColor->color().name()));
	if (ef != EDGE_FONT_SIZE)
		args.append(QString(" -ef %1").arg(ef));
	if (vf != VERTEX_FONT_SIZE)
		args.append(QString(" -vf %1").arg(vf));
	if (_coloredEdges->checkState())
		args.append(QString(" -c"));
	if (_directedGraph->checkState())
		args.append(QString(" -d"));
	if (_legend->checkState())
		args.append(QString(" -l %1").arg(_edgeFontSize->value()));

	if (_nodeDistribution->value() != NODE_DISTRIBUTION)
		args.append(QString(" -nd %1").arg(_nodeDistribution->value()));
	if (_edgeLength->value() != EDGE_LENGTH)
		args.append(QString(" -el %1").arg(_edgeLength->value()));
	if (_edgeCrossings->value() != EDGE_CROSSINGS)
		args.append(QString(" -cr %1").arg(_edgeCrossings->value()));
	if (_initTemp->value() != INIT_TEMP)
		args.append(QString(" -it %1").arg(_initTemp->value()));
	if (_finalTemp->value() != FINAL_TEMP)
		args.append(QString(" -ft %1").arg(_finalTemp->value()));
	if (_coolFactor->value() != COOL_FACTOR)
		args.append(QString(" -cf %1").arg(_coolFactor->value()));
	if (_numSteps->value() != NUM_STEPS)
		args.append(QString(" -ns %1").arg(_numSteps->value()));

	if (_inputEncoding->currentIndex() > 0)
		args.append(QString(" -e %1").arg(_inputEncoding->itemText(
		  _inputEncoding->currentIndex())));

	if (_vertexLabelAttr->count() > 1)
		args.append(QString(" -va %1").arg(_vertexLabelAttr->currentText()));
	if (_edgeLabelAttr->count() > 1)
		args.append(QString(" -ea %1").arg(_edgeLabelAttr->currentText()));

	if (_argumentsEscape->isChecked())
		args = args.replace("#", "\\#");

	_arguments->setText(args);
}

void GUI::getMiscProperties(GraphTab *tab)
{
	setComboBox(_inputEncoding, tab->inputEncoding()->name());
	BLOCK(_antialiasing, setChecked(tab->antialiasing()));
}

void GUI::getAlgorithmProperties(GraphTab *tab)
{
	BLOCK(_sizeSlider, setValue(tab->size()));
	BLOCK(_planaritySlider, setValue(tab->planarity()));
	BLOCK(_qualitySlider, setValue(tab->quality()));

	BLOCK(_nodeDistribution, setValue(tab->nodeDistribution()));
	BLOCK(_edgeLength, setValue(tab->edgeLength()));
	BLOCK(_edgeCrossings, setValue(tab->edgeCrossings()));
	BLOCK(_initTemp, setValue(tab->initTemp()));
	BLOCK(_finalTemp, setValue(tab->finalTemp()));
	BLOCK(_coolFactor, setValue(tab->coolFactor()));
	BLOCK(_numSteps, setValue(tab->numSteps()));
#ifdef SA_LOG_SUPPORT
	BLOCK(_debug, setChecked(tab->logInfo()));
#endif
}

void GUI::getGraphProperties(GraphTab *tab)
{
	initComboBox(_vertexLabelAttr, tab->nodeLabelAttributes());
	initComboBox(_edgeLabelAttr, tab->edgeLabelAttributes());
	setComboBox(_vertexLabelAttr, tab->nodeLabelAttr());
	setComboBox(_edgeLabelAttr, tab->edgeLabelAttr());

	BLOCK(_graphWidth, setValue(tab->dimensions().x()));
	BLOCK(_graphHeight, setValue(tab->dimensions().y()));
	BLOCK(_edgeSize, setValue(tab->edgeSize()));
	BLOCK(_vertexSize, setValue(tab->vertexSize()));
	BLOCK(_edgeColor, setColor(tab->edgeColor()));
	BLOCK(_vertexColor, setColor(tab->vertexColor()));
	BLOCK(_vertexFontSize, setValue(tab->vertexFontSize()));
	BLOCK(_edgeFontSize, setValue(tab->edgeFontSize()));
	BLOCK(_vertexIDs, setChecked(tab->vertexIDs()));
	BLOCK(_edgeValues, setChecked(tab->edgeValues()));
	BLOCK(_coloredEdges, setChecked(tab->coloredEdges()));
	BLOCK(_directedGraph, setChecked(tab->directedGraph()));
	BLOCK(_legend, setChecked(tab->legend()));

	_edgeSize->setMaximum(tab->vertexSize());
	_edgeColor->setEnabled(!tab->coloredEdges());
}

void GUI::zoom(qreal zoom)
{
	_zoom->setText(ZOOM_STRING(zoom));
}

void GUI::writeSettings()
{
	QSettings settings(ORGANIZATION, APPLICATION);

	settings.beginGroup(WINDOW_GROUP);
	settings.setValue(WINDOW_SIZE_SETTING, size());
	settings.setValue(WINDOW_POSITION_SETTING, pos());
	settings.endGroup();

	settings.beginGroup(GRAPH_GROUP);
	settings.setValue(GRAPH_WIDTH_SETTING, _graphWidth->value());
	settings.setValue(GRAPH_HEIGHT_SETTING, _graphHeight->value());
	settings.setValue(EDGE_VALUES_SETTING, _edgeValues->checkState());
	settings.setValue(VERTEX_IDS_SETTING, _vertexIDs->checkState());
	settings.setValue(COLORED_EDGES_SETTING, _coloredEdges->checkState());
	settings.setValue(DIRECTED_GRAPH_SETTING, _directedGraph->checkState());
	settings.setValue(EDGE_SIZE_SETTING, _edgeSize->value());
	settings.setValue(VERTEX_SIZE_SETTING, _vertexSize->value());
	settings.setValue(EDGE_FONT_SIZE_SETTING, _edgeFontSize->value());
	settings.setValue(VERTEX_FONT_SIZE_SETTING, _vertexFontSize->value());
	settings.setValue(EDGE_COLOR_SETTING, _edgeColor->color());
	settings.setValue(VERTEX_COLOR_SETTING, _vertexColor->color());
	settings.setValue(LEGEND_SETTING, _legend->checkState());
	settings.endGroup();

	settings.beginGroup(ALGORITHM_GROUP);
	settings.setValue(SIZE_SLIDER_SETTING, _sizeSlider->value());
	settings.setValue(PLANARITY_SLIDER_SETTING, _planaritySlider->value());
	settings.setValue(QUALITY_SLIDER_SETTING, _qualitySlider->value());
	settings.setValue(NODE_DISTRIBUTION_SETTING, _nodeDistribution->value());
	settings.setValue(EDGE_LENGTH_SETTING, _edgeLength->value());
	settings.setValue(EDGE_CROSSINGS_SETTING, _edgeCrossings->value());
	settings.setValue(INIT_TEMP_SETTING, _initTemp->value());
	settings.setValue(FINAL_TEMP_SETTING, _finalTemp->value());
	settings.setValue(COOL_FACTOR_SETTING, _coolFactor->value());
	settings.setValue(NUM_STEPS_SETTING, _numSteps->value());
	settings.endGroup();

	settings.beginGroup(MISC_GROUP);
	settings.setValue(INPUT_ENCODING_SETTING, _inputEncoding->currentText());
	settings.setValue(ANTIALIASING_SETTING, _antialiasing->checkState());
	settings.setValue(ESCAPE_SPECIALS_SETTING, _argumentsEscape->checkState());
	settings.endGroup();
}

void GUI::readSettings()
{
	QSettings settings(ORGANIZATION, APPLICATION);

	settings.beginGroup(WINDOW_GROUP);
	resize(settings.value(WINDOW_SIZE_SETTING, QSize(800, 600)).toSize());
	move(settings.value(WINDOW_POSITION_SETTING, QPoint(100, 100)).toPoint());
	settings.endGroup();

	settings.beginGroup(GRAPH_GROUP);
	_graphWidth->setValue(settings.value(GRAPH_WIDTH_SETTING,
	  GRAPH_WIDTH).toInt());
	_graphHeight->setValue(settings.value(GRAPH_HEIGHT_SETTING,
	  GRAPH_HEIGHT).toInt());
	_vertexIDs->setCheckState((Qt::CheckState)settings.value(
	  VERTEX_IDS_SETTING, Qt::Checked).toInt());
	_edgeValues->setCheckState((Qt::CheckState)settings.value(
	  EDGE_VALUES_SETTING, Qt::Unchecked).toInt());
	_coloredEdges->setCheckState((Qt::CheckState)settings.value(
	  COLORED_EDGES_SETTING, Qt::Unchecked).toInt());
	_directedGraph->setCheckState((Qt::CheckState)settings.value(
	  DIRECTED_GRAPH_SETTING, Qt::Unchecked).toInt());
	_vertexSize->setValue(settings.value(VERTEX_SIZE_SETTING,
	  VERTEX_SIZE).toInt());
	_edgeSize->setValue(settings.value(EDGE_SIZE_SETTING, EDGE_SIZE).toInt());
	_edgeSize->setMaximum(_vertexSize->value());
	_edgeFontSize->setValue(settings.value(EDGE_FONT_SIZE_SETTING,
	  EDGE_FONT_SIZE).toInt());
	_vertexFontSize->setValue(settings.value(VERTEX_FONT_SIZE_SETTING,
	  VERTEX_FONT_SIZE).toInt());
	_edgeColor->setColor(settings.value(EDGE_COLOR_SETTING,
	  QColor(EDGE_COLOR)).value<QColor>());
	_vertexColor->setColor(settings.value(VERTEX_COLOR_SETTING,
	  QColor(VERTEX_COLOR)).value<QColor>());
	_legend->setCheckState((Qt::CheckState)settings.value(
	  LEGEND_SETTING, Qt::Unchecked).toInt());
	_legend->setEnabled(_coloredEdges->isChecked());
	settings.endGroup();

	settings.beginGroup(ALGORITHM_GROUP);
	_sizeSlider->setValue(settings.value(SIZE_SLIDER_SETTING, 50).toInt());
	_planaritySlider->setValue(settings.value(PLANARITY_SLIDER_SETTING,
	  50).toInt());
	_qualitySlider->setValue(settings.value(QUALITY_SLIDER_SETTING,
	  50).toInt());
	_nodeDistribution->setValue(settings.value(NODE_DISTRIBUTION_SETTING,
	  NODE_DISTRIBUTION).toFloat());
	_edgeLength->setValue(settings.value(EDGE_LENGTH_SETTING,
	  EDGE_LENGTH).toFloat());
	_edgeCrossings->setValue(settings.value(EDGE_CROSSINGS_SETTING,
	  EDGE_CROSSINGS).toFloat());
	_initTemp->setValue(settings.value(INIT_TEMP_SETTING, INIT_TEMP).toFloat());
	_finalTemp->setValue(settings.value(FINAL_TEMP_SETTING,
	  FINAL_TEMP).toFloat());
	_coolFactor->setValue(settings.value(COOL_FACTOR_SETTING,
	  COOL_FACTOR).toFloat());
	_numSteps->setValue(settings.value(NUM_STEPS_SETTING, NUM_STEPS).toInt());
	settings.endGroup();

	settings.beginGroup(MISC_GROUP);
	int index = 0;
	QString ie = settings.value(INPUT_ENCODING_SETTING,
	  (*encodings)->name()).toString();
	for (Encoding **ep = encodings; *ep; ep++, index++) {
		if (!strcmp((*ep)->name(), ie.toLatin1().constData())) {
			_inputEncoding->setCurrentIndex(index);
			break;
		}
	}
	_antialiasing->setChecked((Qt::CheckState)settings.value(
	  ANTIALIASING_SETTING, Qt::Checked).toBool());
	_argumentsEscape->setChecked((Qt::CheckState)settings.value(
	  ESCAPE_SPECIALS_SETTING, Qt::Unchecked).toBool());
	settings.endGroup();

	getArguments();
}

void GUI::closeEvent(QCloseEvent *event)
{
	writeSettings();
	event->accept();
}
