#include "GUI/gui.h"
#include "GUI/colorcombobox.h"
#include "GUI/numericedit.h"
#include "GUI/graphtab.h"
#include "IO/io.h"
#include "IO/encoding.h"
#include "IO/modules.h"
#include "CORE/config.h"
#include "GUI/icons.h"


#define TAB() ((GraphTab*) _viewTab->currentWidget())

#define BLOCK(widget, action) \
	widget->blockSignals(true); \
	widget->action; \
	widget->blockSignals(false)

#define ZOOM_STRING(zoom) QString("%1%").arg((int)((zoom) * 100))


static QString saveFilter(OutputProvider *provider);
static QString errorDescription(IO::Error error);


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
}

void GUI::createProperties()
{
	createGraphProperties();
	createSAProperties();
	createIOProperties();

	_properties = new QToolBox;

	_properties->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,
	  QSizePolicy::Ignored));
	_properties->setMinimumWidth(qMax(_IOProperties->sizeHint().width(),
	  qMax(_graphProperties->sizeHint().width(),
	  _SAProperties->sizeHint().width())));

	_properties->addItem(_graphProperties, tr("Graph settings"));
	_properties->addItem(_SAProperties, tr("Algorithm settings"));
	_properties->addItem(_IOProperties, tr("I/O settings"));
}

void GUI::createIOProperties()
{
	QGroupBox *encodingBox = new QGroupBox(tr("Encoding"));

	_inputEncoding = new QComboBox();

	for (Encoding **ep = encodings; *ep; ep++)
		_inputEncoding->addItem((*ep)->name());

	connect(_inputEncoding, SIGNAL(activated(int)), this,
	  SLOT(setInputEncoding(int)));

	QFormLayout *encodingLayout = new QFormLayout;
	encodingLayout->addRow(tr("Input encoding:"), _inputEncoding);
	encodingBox->setLayout(encodingLayout);


	QGroupBox *displayBox = new QGroupBox(tr("Display"));

	_antialiasing = new QCheckBox(tr("Use antialiasing"), this);
	QVBoxLayout *displayLayout = new QVBoxLayout;
	displayLayout->addWidget(_antialiasing, 0, Qt::AlignTop);
	displayBox->setLayout(displayLayout);

	connect(_antialiasing, SIGNAL(stateChanged(int)),
	  this, SLOT(setAntialiasing(int)));


	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(encodingBox);
	layout->addWidget(displayBox);

	_IOProperties = new QWidget;
	_IOProperties->setLayout(layout);
}

void GUI::createSAProperties()
{
	QGroupBox *graphBox = new QGroupBox(tr("Graph layout"));

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
	graphBox->setLayout(graphLayout);


	QGroupBox *SABox = new QGroupBox(tr("SA parameters"));

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
	SABox->setLayout(SAlayout);

#ifdef SA_LOG_SUPPORT
	QGroupBox *debugBox = new QGroupBox(tr("Debug"));
	_debug = new QCheckBox(tr("Create debug output"), this);
	QVBoxLayout *debugLayout = new QVBoxLayout;
	debugLayout->addWidget(_debug, 0, Qt::AlignTop);
	debugBox->setLayout(debugLayout);

	connect(_debug, SIGNAL(stateChanged(int)),
	  this, SLOT(setSALogInfo(int)));
#endif


	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(graphBox);
	layout->addWidget(SABox);
#ifdef SA_LOG_SUPPORT
	layout->addWidget(debugBox);
#endif

	_SAProperties = new QWidget;
	_SAProperties->setLayout(layout);
}

void GUI::createGraphProperties()
{
	_edgeValues = new QCheckBox(tr("Show edge values"), this);
	_vertexIDs = new QCheckBox(tr("Show vertex IDs"), this);
	_coloredEdges = new QCheckBox(tr("Colored edges"), this);

	connect(_edgeValues, SIGNAL(stateChanged(int)),
	  this, SLOT(showEdgeValues(int)));
	connect(_vertexIDs, SIGNAL(stateChanged(int)),
	  this, SLOT(showVertexIDs(int)));
	connect(_coloredEdges, SIGNAL(stateChanged(int)),
	  this, SLOT(colorizeEdges(int)));

	_graphWidth = new QSpinBox();
	_graphHeight = new QSpinBox();
	_graphWidth->setMaximum(10000);
	_graphHeight->setMaximum(10000);

	connect(_graphWidth, SIGNAL(valueChanged(int)),
	  this, SLOT(setGraphWidth(int)));
	connect(_graphHeight, SIGNAL(valueChanged(int)),
	  this, SLOT(setGraphHeight(int)));

	QGroupBox *graphBox = new QGroupBox(tr("Graph"));

	QVBoxLayout *show = new QVBoxLayout;
	show->addWidget(_vertexIDs);
	show->addWidget(_edgeValues);
	show->addWidget(_coloredEdges);

	QFormLayout *dimensions = new QFormLayout;
	dimensions->addRow(tr("Width:"), _graphWidth);
	dimensions->addRow(tr("Height:"), _graphHeight);

	QVBoxLayout *graphLayout = new QVBoxLayout;
	graphLayout->setAlignment(Qt::AlignTop);

	graphLayout->addLayout(dimensions);
	graphLayout->addSpacing(10);
	graphLayout->addLayout(show);

	graphBox->setLayout(graphLayout);


	_vertexSize = new QSpinBox();
	_edgeSize = new QSpinBox();
	_edgeFontSize = new QSpinBox();
	_vertexFontSize = new QSpinBox();
	_edgeColor = new ColorComboBox();
	_vertexColor = new ColorComboBox();

	_edgeFontSize->setMinimum(MIN_FONT_SIZE);
	_vertexFontSize->setMinimum(MIN_FONT_SIZE);

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


	QGroupBox *vertexBox = new QGroupBox(tr("Vertexes"));
	QGroupBox *edgeBox = new QGroupBox(tr("Edges"));

	QFormLayout *edgeLayout = new QFormLayout;
	QFormLayout *vertexLayout = new QFormLayout;

	edgeLayout->addRow(tr("Size:"), _edgeSize);
	edgeLayout->addRow(tr("Font size:"), _edgeFontSize);
	edgeLayout->addRow(tr("Color:"), _edgeColor);
	edgeBox->setLayout(edgeLayout);

	vertexLayout->addRow(tr("Size:"), _vertexSize);
	vertexLayout->addRow(tr("Font size:"), _vertexFontSize);
	vertexLayout->addRow(tr("Color:"), _vertexColor);
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
	getIOProperties(tab);

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
	setIOProperties(tab);
	IO::Error error = tab->readGraph(fileName);

	if (error) {
		QMessageBox::critical(this, tr("Error"), tr("Error loading graph")
		  + QString(":\n") + errorDescription(error));
		delete tab;
	} else {
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

	_viewTab->removeTab(_viewTab->indexOf(tab));
	delete tab;

	int cnt = _viewTab->count();
	if (cnt == 0) {
		_fileActionGroup->setEnabled(false);
		_graphActionGroup->setEnabled(false);
	} else if (cnt == 1)
		_projectActionGroup->setEnabled(false);
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

#ifdef SA_LOG_SUPPORT
void GUI::setSALogInfo(int state)
{
	if (TAB())
		TAB()->setLogInfo((state == Qt::Checked) ? true : false);
}
#endif

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

void GUI::colorizeEdges(int state)
{
	if (TAB())
		TAB()->colorizeEdges((state == Qt::Checked) ? true : false);
	_edgeColor->setEnabled((state == Qt::Checked) ? false : true);
}

void GUI::setInputEncoding(int index)
{
	if (TAB())
		TAB()->setInputEncoding(*(encodings + index));
}

void GUI::setAntialiasing(int state)
{
	if (TAB())
		TAB()->setAntialiasing((state == Qt::Checked) ? true : false);
}

void GUI::setIOProperties(GraphTab *tab)
{
	Encoding* encoding = *(encodings + _inputEncoding->currentIndex());

	tab->setInputEncoding(encoding);
	tab->setAntialiasing((_antialiasing->checkState() == Qt::Checked)
	  ? true : false);
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
#ifdef SA_LOG_SUPPORT
	tab->setLogInfo((_debug->checkState() == Qt::Checked) ? true : false);
#endif
}

void GUI::setGraphProperties(GraphTab *tab)
{
	tab->showVertexIDs((_vertexIDs->checkState() == Qt::Checked)
	  ? true : false);
	tab->showEdgeValues((_edgeValues->checkState() == Qt::Checked)
	  ? true : false);
	tab->colorizeEdges((_coloredEdges->checkState() == Qt::Checked)
	  ? true : false);
	tab->setDimensions(QPoint(_graphWidth->value(), _graphHeight->value()));
	tab->setEdgeSize(_edgeSize->value());
	tab->setVertexSize(_vertexSize->value());
	tab->setEdgeColor(_edgeColor->color());
	tab->setVertexColor(_vertexColor->color());
	tab->setVertexFontSize(_vertexFontSize->value());
	tab->setEdgeFontSize(_edgeFontSize->value());
}

void GUI::getIOProperties(GraphTab *tab)
{
	int index = 0;

	for (Encoding **e = encodings; *e; e++, index++)
		if (!strcmp((*e)->name(), tab->inputEncoding()->name()))
			break;

	BLOCK(_inputEncoding, setCurrentIndex(index));
	BLOCK(_antialiasing, setChecked(tab->antialiasing()));
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
#ifdef SA_LOG_SUPPORT
	BLOCK(_debug, setChecked(tab->logInfo()));
#endif
}

void GUI::getGraphProperties(GraphTab *tab)
{
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

	_edgeSize->setMaximum(tab->vertexSize());
	_edgeColor->setEnabled(!tab->coloredEdges());
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

	settings.beginGroup("Graph");
	settings.setValue("width", _graphWidth->value());
	settings.setValue("height", _graphHeight->value());
	settings.setValue("edgeValues", _edgeValues->checkState());
	settings.setValue("vertexIDs", _vertexIDs->checkState());
	settings.setValue("coloredEdges", _coloredEdges->checkState());
	settings.setValue("edgeSize", _edgeSize->value());
	settings.setValue("vertexSize", _vertexSize->value());
	settings.setValue("edgeFontSize", _edgeFontSize->value());
	settings.setValue("vertexFontSize", _vertexFontSize->value());
	settings.setValue("edgeColor", _edgeColor->color());
	settings.setValue("vertexColor", _vertexColor->color());
	settings.endGroup();

	settings.beginGroup("Algorithm");
	settings.setValue("nodeDistribution", _nodeDistribution->value());
	settings.setValue("edgeLength", _edgeLength->value());
	settings.setValue("edgeCrossings", _edgeCrossings->value());
	settings.setValue("initTemp", _initTemp->value());
	settings.setValue("finalTemp", _finalTemp->value());
	settings.setValue("coolFactor", _coolFactor->value());
	settings.setValue("numSteps", _numSteps->value());
	settings.endGroup();

	settings.beginGroup("IO");
	settings.setValue("inputEncoding", _inputEncoding->currentText());
	settings.setValue("antialiasing", _antialiasing->checkState());
	settings.endGroup();
}

void GUI::readSettings()
{
	QSettings settings("Hypercube", "Hypercube");

	settings.beginGroup("MainWindow");
	resize(settings.value("size", QSize(800, 600)).toSize());
	move(settings.value("pos", QPoint(100, 100)).toPoint());
	settings.endGroup();

	settings.beginGroup("Graph");
	_graphWidth->setValue(settings.value("width", GRAPH_WIDTH).toInt());
	_graphHeight->setValue(settings.value("height", GRAPH_HEIGHT).toInt());
	_vertexIDs->setCheckState((Qt::CheckState)settings.value("vertexIDs",
	  Qt::Checked).toInt());
	_edgeValues->setCheckState((Qt::CheckState)settings.value("edgeValues",
	  Qt::Unchecked).toInt());
	_coloredEdges->setCheckState((Qt::CheckState)settings.value("coloredEdges",
	  Qt::Unchecked).toInt());
	_vertexSize->setValue(settings.value("vertexSize", VERTEX_SIZE).toInt());
	_edgeSize->setValue(settings.value("edgeSize", EDGE_SIZE).toInt());
	_edgeSize->setMaximum(_vertexSize->value());
	_edgeFontSize->setValue(settings.value("edgeFontSize",
	  EDGE_FONT_SIZE).toInt());
	_vertexFontSize->setValue(settings.value("vertexFontSize",
	  VERTEX_FONT_SIZE).toInt());
	_edgeColor->setColor(settings.value("edgeColor",
	  QColor(EDGE_COLOR)).value<QColor>());
	_vertexColor->setColor(settings.value("vertexColor",
	  QColor(VERTEX_COLOR)).value<QColor>());
	settings.endGroup();

	settings.beginGroup("Algorithm");
	_nodeDistribution->setValue(settings.value("nodeDistribution",
	  NODE_DISTRIBUTION).toFloat());
	_edgeLength->setValue(settings.value("edgeLength",
	  EDGE_LENGTH).toFloat());
	_edgeCrossings->setValue(settings.value("edgeCrossings",
	  EDGE_CROSSINGS).toFloat());
	_initTemp->setValue(settings.value("initTemp", INIT_TEMP).toFloat());
	_finalTemp->setValue(settings.value("finalTemp", FINAL_TEMP).toFloat());
	_coolFactor->setValue(settings.value("coolFactor", COOL_FACTOR).toFloat());
	_numSteps->setValue(settings.value("numSteps", NUM_STEPS).toInt());
	settings.endGroup();

	settings.beginGroup("IO");
	int index = 0;
	QString ie = settings.value("inputEncoding",
	  (*encodings)->name()).toString();
	for (Encoding **ep = encodings; *ep; ep++, index++) {
		if (!strcmp((*ep)->name(), ie.toAscii().constData())) {
			_inputEncoding->setCurrentIndex(index);
			break;
		}
	}
	_antialiasing->setChecked((Qt::CheckState)settings.value("antialiasing",
	  Qt::Unchecked).toBool());
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
