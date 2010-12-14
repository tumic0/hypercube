#include "GUI/gui.h"
#include "GUI/colorcombobox.h"
#include "GUI/numericedit.h"
#include "GUI/graphtab.h"
#include "IO/io.h"
#include "CORE/config.h"


/*!
	\class GUI
	\brief The application's graphical user interface.

	The GUI class provides the application's graphical user interface.
*/


#define TAB() ((GraphTab*) _viewTab->currentWidget())

#define BLOCK(widget, action) \
	widget->blockSignals(true); \
	widget->action; \
	widget->blockSignals(false)

#define ZOOM_STRING(zoom) QString("%1%").arg((int)((zoom) * 100))


static QString saveFilter(OutputProvider *provider);
static QString errorDescription(IO::Error error);


/*!
	Constructs the application's graphical user interface.
*/
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

	_masterTab = NULL;

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
	_projectAction = new QAction(QIcon(QPixmap(PROJECT_GRAPH_ICON)),
	  tr("Project this to all graphs"), this);
	_projectAction->setActionGroup(_graphActionGroup);
	_projectAction->setCheckable(true);
	connect(_projectAction, SIGNAL(triggered(bool)), this,
	  SLOT(projectGraph(bool)));
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
	_graphMenu->addSeparator();
	_graphMenu->addAction(_projectAction);

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
	_graphToolBar->addSeparator();
	_graphToolBar->addAction(_projectAction);
}

void GUI::createProperties()
{
	createGraphProperties();
	createSAProperties();

	_properties = new QToolBox;

	_properties->addItem(_graphProperties, tr("Graph settings"));
	_properties->addItem(_SAProperties, tr("SA settings"));

	_properties->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,
	  QSizePolicy::Minimum));
	_properties->setMinimumWidth(qMax(_graphProperties->sizeHint().width(),
	  _SAProperties->sizeHint().width()));
}

void GUI::createSAProperties()
{
	QGroupBox *graphBox = new QGroupBox(tr("Graph layout"));

	_nodeDistribution = new FloatEdit(10);
	_nodeDistribution->setValue(NODE_DISTRIBUTION);
	_edgeLength = new FloatEdit(10);
	_edgeLength->setValue(EDGE_LENGTH);
	_edgeCrossings = new FloatEdit(10);
	_edgeCrossings->setValue(EDGE_CROSSINGS);

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
	graphBox->setLayout(graphLayout);


	QGroupBox *SABox = new QGroupBox(tr("SA parameters"));

	_initTemp = new FloatEdit(10);
	_initTemp->setValue(INIT_TEMP);
	_finalTemp = new FloatEdit(10);
	_finalTemp->setValue(FINAL_TEMP);
	_coolFactor = new FloatEdit(10);
	_coolFactor->setValue(COOL_FACTOR);
	_numSteps = new IntEdit(10);
	_numSteps->setValue(NUM_STEPS);

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
	SABox->setLayout(SAlayout);

	QGroupBox *debugBox = new QGroupBox(tr("Debug"));
	_debug = new QCheckBox("Create debug output", this);
	QVBoxLayout *debugLayout = new QVBoxLayout;
	debugLayout->addWidget(_debug, 0, Qt::AlignTop);
	debugBox->setLayout(debugLayout);

	connect(_debug, SIGNAL(stateChanged(int)),
	  this, SLOT(setSALogInfo(int)));


	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(graphBox);
	layout->addWidget(SABox);
	layout->addWidget(debugBox);

	_SAProperties = new QWidget;
	_SAProperties->setLayout(layout);
}

void GUI::createGraphProperties()
{
	// Graph properties control widgets
	_edgeValues = new QCheckBox(tr("Edge values"), this);
	_vertexIDs = new QCheckBox(tr("Vertex IDs"), this);
	_vertexIDs->setCheckState(Qt::Checked);

	connect(_edgeValues, SIGNAL(stateChanged(int)),
	  this, SLOT(showEdgeValues(int)));
	connect(_vertexIDs, SIGNAL(stateChanged(int)),
	  this, SLOT(showVertexIDs(int)));

	_graphWidth = new QSpinBox();
	_graphHeight = new QSpinBox();
	_graphWidth->setMaximum(10000);
	_graphHeight->setMaximum(10000);
	_graphWidth->setValue(GRAPH_WIDTH);
	_graphHeight->setValue(GRAPH_HEIGHT);

	connect(_graphWidth, SIGNAL(valueChanged(int)),
	  this, SLOT(setGraphWidth(int)));
	connect(_graphHeight, SIGNAL(valueChanged(int)),
	  this, SLOT(setGraphHeight(int)));

	// Graph properties box layout
	QGroupBox *propertiesBox = new QGroupBox(tr("Graph properties"));

	QFormLayout *dimensions = new QFormLayout;
	dimensions->addRow(tr("Width:"), _graphWidth);
	dimensions->addRow(tr("Height:"), _graphHeight);

	QLabel *dimensionsLabel = new QLabel(QString("<i>") + tr("Graph size")
	  + QString("</i>"));
	QLabel *elementsLabel = new QLabel(QString("<i>") + tr("Display")
	  + QString("</i>"));

	QVBoxLayout *propertiesLayout = new QVBoxLayout;
	propertiesLayout->addWidget(dimensionsLabel);
	propertiesLayout->addLayout(dimensions);
	propertiesLayout->addWidget(elementsLabel);
	propertiesLayout->addWidget(_vertexIDs);
	propertiesLayout->addWidget(_edgeValues);
	propertiesLayout->addStretch(1);
	propertiesBox->setLayout(propertiesLayout);


	// Appearance control widgets
	_vertexSize = new QSpinBox();
	_edgeSize = new QSpinBox();
	_edgeFontSize = new QSpinBox();
	_vertexFontSize = new QSpinBox();
	_edgeColor = new ColorComboBox();
	_vertexColor = new ColorComboBox();

	_vertexSize->setValue(VERTEX_SIZE);
	_edgeSize->setValue(EDGE_SIZE);
	_edgeSize->setMaximum(_vertexSize->value());
	_edgeFontSize->setValue(EDGE_FONT_SIZE);
	_edgeFontSize->setMinimum(MIN_FONT_SIZE);
	_vertexFontSize->setValue(VERTEX_FONT_SIZE);
	_vertexFontSize->setMinimum(MIN_FONT_SIZE);
	_edgeColor->setColor(QColor(EDGE_COLOR));
	_vertexColor->setColor(QColor(VERTEX_COLOR));

	connect(_edgeSize, SIGNAL(valueChanged(int)),
	  this, SLOT(setEdgeSize(int)));
	connect(_vertexSize, SIGNAL(valueChanged(int)),
	  this, SLOT(setVertexSize(int)));
	connect(_edgeFontSize, SIGNAL(valueChanged(int)),
	  this, SLOT(setEdgeFontSize(int)));
	connect(_vertexFontSize, SIGNAL(valueChanged(int)),
	  this, SLOT(setVertexFontSize(int)));

	connect(_edgeColor, SIGNAL(activated(const QColor&)),
	  this, SLOT(setEdgeColor(const QColor&)));
	connect(_vertexColor, SIGNAL(activated(const QColor&)),
	  this, SLOT(setVertexColor(const QColor&)));


	// Appearance box layout
	QGroupBox *appearanceBox = new QGroupBox(tr("Graph appearance"));

	QFormLayout *edge = new QFormLayout;
	QFormLayout *vertex = new QFormLayout;

	edge->addRow(tr("Size:"), _edgeSize);
	edge->addRow(tr("Font size:"), _edgeFontSize);
	edge->addRow(tr("Color:"), _edgeColor);

	vertex->addRow(tr("Size:"), _vertexSize);
	vertex->addRow(tr("Font size:"), _vertexFontSize);
	vertex->addRow(tr("Color:"), _vertexColor);

	QLabel *edgeLabel = new QLabel(QString("<i>") + tr("Edges")
	  + QString("</i>"));
	QLabel *vertexLabel = new QLabel(QString("<i>") + tr("Vertexes")
	  + QString("</i>"));

	QVBoxLayout *appearanceLayout = new QVBoxLayout();
	appearanceLayout->addWidget(vertexLabel);
	appearanceLayout->addLayout(vertex);
	appearanceLayout->addWidget(edgeLabel);
	appearanceLayout->addLayout(edge);
	appearanceLayout->addStretch(1);
	appearanceBox->setLayout(appearanceLayout);


	// Graph settings tab layout
	QVBoxLayout *layout = new QVBoxLayout;

	layout->addWidget(propertiesBox);
	layout->addWidget(appearanceBox);

	_graphProperties = new QWidget;
	_graphProperties->setLayout(layout);
}

void GUI::createViews()
{
	_viewTab = new QTabWidget;

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

void GUI::tabChanged(int current)
{
	if (current == -1) {
		_fileName->setText(QString::null);
		_zoom->setText(QString::null);
		return;
	}

	GraphTab *tab = (GraphTab*) _viewTab->widget(current);

	getSAProperties(tab);
	getGraphProperties(tab);

	if (_masterTab && tab != _masterTab)
		tab->project(_masterTab->graph());

	_graphActionGroup->setEnabled(tab->enabled());
	_graphProperties->setEnabled(tab->enabled());

	_zoom->setText(ZOOM_STRING(tab->view()->zoom()));
	_fileName->setText(tab->fileName());
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
	QString fileName = QFileDialog::getOpenFileName(this);
	if (fileName.isEmpty())
		return;

	GraphTab *tab = new GraphTab();

	setSAProperties(tab);
	setGraphProperties(tab);
	IO::Error error = tab->readGraph(fileName);

	if (error) {
		QMessageBox::critical(this, tr("Error"), tr("Error loading graph")
		  + QString(":\n") + errorDescription(error));
		delete tab;
	} else {
		if (_masterTab)
			tab->setEnabled(false);

		QFileInfo fi(fileName);
		int index = _viewTab->addTab(tab, fi.fileName());

		connect(tab->view(), SIGNAL(zoomed(qreal)), SLOT(zoom(qreal)));

		_viewTab->setCurrentIndex(index);

		if (_viewTab->count() == 1) {
			_fileActionGroup->setEnabled(true);
			_graphActionGroup->setEnabled(true);
		}
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
	  QString::null, QString::null, filter, &selectedFilter);
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

		IO::Error error = TAB()->writeGraph(fileName, *provider);
		if (error)
			QMessageBox::critical(this, tr("Error"),
			  tr("Error saving graph") + QString(":\n")
			  + errorDescription(error));
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

	if (tab == _masterTab) {
		for (int i = 0; i < _viewTab->count(); i++)
			((GraphTab*) _viewTab->widget(i))->setEnabled(true);

		_masterTab = NULL;
		_projectAction->setChecked(false);
	}

	_viewTab->removeTab(_viewTab->indexOf(tab));
	delete tab;

	if (_viewTab->count() == 0) {
		_fileActionGroup->setEnabled(false);
		_graphActionGroup->setEnabled(false);
	}
}

void GUI::transformGraph()
{
	TAB()->transformGraph();
}

void GUI::reloadGraph()
{
	IO::Error error = TAB()->readGraph();
	if (error) {
		QMessageBox::critical(this, tr("Error"),
		  tr("Error loading graph") + QString(":\n")
		  + errorDescription(error));
		closeFile();
	}
}

void GUI::projectGraph(bool checked)
{
	GraphTab* tab;

	for (int i = 0; i < _viewTab->count(); i++) {
		if (_viewTab->currentIndex() == i)
			continue;
		tab = ((GraphTab*) _viewTab->widget(i));

		if (checked) {
			tab->project(TAB()->graph());
			tab->setEnabled(false);
		} else {
			tab->setEdgeColor(tab->edgeColor());
			tab->setEdgeSize(tab->edgeSize());
			tab->setEdgeFontSize(tab->edgeFontSize());
			tab->setEdgeZValue(-1);
			tab->setEnabled(true);
		}
	}

	_masterTab = (checked) ? TAB() : NULL;
}


void GUI::setNodeDistribution(float value)
{
	if (TAB())
		TAB()->setNodeDistribution(value);
}

void GUI::setEdgeLength(float value)
{
	if (TAB())
		TAB()->setEdgeLength(value);
}

void GUI::setEdgeCrossings(float value)
{
	if (TAB())
		TAB()->setEdgeCrossings(value);
}

void GUI::setInitTemp(float value)
{
	if (TAB())
		TAB()->setInitTemp(value);
}

void GUI::setFinalTemp(float value)
{
	if (TAB())
		TAB()->setFinalTemp(value);
}

void GUI::setCoolFactor(float value)
{
	if (TAB())
		TAB()->setCoolFactor(value);
}

void GUI::setNumSteps(int value)
{
	if (TAB())
		TAB()->setNumSteps(value);
}

void GUI::setSALogInfo(int state)
{
	if (TAB())
		TAB()->setLogInfo((state == Qt::Checked) ? true : false);
}


void GUI::setGraphWidth(int width)
{
	if (TAB())
		TAB()->setDimensions(QPoint(width, TAB()->dimensions().y()));
}

void GUI::setGraphHeight(int height)
{
	if (TAB())
		TAB()->setDimensions(QPoint(TAB()->dimensions().x(), height));
}

void GUI::setVertexColor(const QColor &color)
{
	if (TAB())
		TAB()->setVertexColor(color);
}

void GUI::setEdgeColor(const QColor &color)
{
	if (TAB())
		TAB()->setEdgeColor(color);
}

void GUI::setVertexSize(int size)
{
	if (TAB())
		TAB()->setVertexSize(size);
	_edgeSize->setMaximum(size);
}

void GUI::setEdgeSize(int size)
{
	if (TAB())
		TAB()->setEdgeSize(size);
}

void GUI::setEdgeFontSize(int size)
{
	if (TAB())
		TAB()->setEdgeFontSize(size);
}

void GUI::setVertexFontSize(int size)
{
	if (TAB())
		TAB()->setVertexFontSize(size);
}

void GUI::showEdgeValues(int state)
{
	if (TAB())
		TAB()->showEdgeValues((state == Qt::Checked) ? true : false);
}

void GUI::showVertexIDs(int state)
{
	if (TAB())
		TAB()->showVertexIDs((state == Qt::Checked) ? true : false);
}

void GUI::setSAProperties(GraphTab *tab)
{
	tab->setNodeDistribution(_nodeDistribution->value());
	tab->setEdgeLength(_edgeLength->value());
	tab->setEdgeCrossings(_edgeCrossings->value());
	tab->setInitTemp(_initTemp->value());
	tab->setFinalTemp(_finalTemp->value());
	tab->setCoolFactor(_coolFactor->value());
	tab->setNumSteps(_numSteps->value());
	tab->setLogInfo((_debug->checkState() == Qt::Checked) ? true : false);
}

void GUI::setGraphProperties(GraphTab *tab)
{
	tab->setDimensions(QPoint(_graphWidth->value(), _graphHeight->value()));
	tab->setEdgeSize(_edgeSize->value());
	tab->setVertexSize(_vertexSize->value());
	tab->setEdgeColor(_edgeColor->color());
	tab->setVertexColor(_vertexColor->color());
	tab->setVertexFontSize(_vertexFontSize->value());
	tab->setEdgeFontSize(_edgeFontSize->value());
	tab->showVertexIDs((_vertexIDs->checkState() == Qt::Checked)
	  ? true : false);
	tab->showEdgeValues((_edgeValues->checkState() == Qt::Checked)
	  ? true : false);
}

void GUI::getSAProperties(GraphTab *tab)
{
	_nodeDistribution->setValue(tab->nodeDistribution());
	_edgeLength->setValue(tab->edgeLength());
	_edgeCrossings->setValue(tab->edgeCrossings());
	_initTemp->setValue(tab->initTemp());
	_finalTemp->setValue(tab->finalTemp());
	_coolFactor->setValue(tab->coolFactor());
	_numSteps->setValue(tab->numSteps());
	BLOCK(_debug, setChecked(tab->logInfo()));
}

void GUI::getGraphProperties(GraphTab *tab)
{
	BLOCK(_graphWidth, setValue(tab->dimensions().x()));
	BLOCK(_graphHeight, setValue(tab->dimensions().y()));
	BLOCK(_edgeSize, setValue(tab->edgeSize()));
	_edgeSize->setMaximum(tab->vertexSize());
	BLOCK(_vertexSize, setValue(tab->vertexSize()));
	BLOCK(_edgeColor, setColor(tab->edgeColor()));
	BLOCK(_vertexColor, setColor(tab->vertexColor()));
	BLOCK(_vertexFontSize, setValue(tab->vertexFontSize()));
	BLOCK(_edgeFontSize, setValue(tab->edgeFontSize()));
	BLOCK(_vertexIDs, setChecked(tab->vertexIDs()));
	BLOCK(_edgeValues, setChecked(tab->edgeValues()));
}

void GUI::zoom(qreal zoom)
{
	_zoom->setText(ZOOM_STRING(zoom));
}

void GUI::writeSettings()
{
	QSettings settings("Hypercube", "Hypercube");

	settings.beginGroup("MainWindow");
	settings.setValue("size", size());
	settings.setValue("pos", pos());
	settings.endGroup();
}

void GUI::readSettings()
{
	QSettings settings("Hypercube", "Hypercube");

	settings.beginGroup("MainWindow");
	resize(settings.value("size", QSize(800, 600)).toSize());
	move(settings.value("pos", QPoint(100, 100)).toPoint());
	settings.endGroup();
}

void GUI::closeEvent(QCloseEvent *event)
{
	writeSettings();
	event->accept();
}


QString saveFilter(OutputProvider *provider)
{
	return QString(provider->description()) + QString(" (*.")
	  + QString(provider->type()) + QString(")");
}

QString errorDescription(IO::Error error)
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
