#include "MainWindow.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QToolBar>
#include <QStatusBar>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QFileDialog>
#include <QImage>
#include <QPainter>
#include <QSvgGenerator>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QDateTime>
#include <QRegularExpression>

#include "../view/TopologyScene.h"
#include "../view/TopologyView.h"
#include "../core/TopologyGenerator.h"
#include "../core/Params.h"

// Helper to parse a string of numbers (comma or space separated)
static std::vector<double> parseDoubles(const QString &s)
{
	std::vector<double> vec;
	const auto parts = s.split(QRegularExpression("[\\s,]+"), Qt::SkipEmptyParts);
	for (const auto &part : parts)
	{
		bool ok;
		double val = part.toDouble(&ok);
		if (ok)
			vec.push_back(val);
	}
	return vec;
}

static std::vector<int> parseInts(const QString &s)
{
	std::vector<int> vec;
	const auto parts = s.split(QRegularExpression("[\\s,]+"), Qt::SkipEmptyParts);
	for (const auto &part : parts)
	{
		bool ok;
		int val = part.toInt(&ok);
		if (ok)
			vec.push_back(val);
	}
	return vec;
}

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), m_scene(new TopologyScene(this)), m_view(new TopologyView())
{
	m_view->setScene(m_scene);
	buildUi();
	connectSignals();
	m_concentricGroup->setChecked(true);
	onParamsChanged(); // Initial generation
}

MainWindow::~MainWindow() = default;

void MainWindow::buildUi()
{
	auto *central = new QWidget(this);
	setCentralWidget(central);

	// --- Control Panels ---
	setupConcentricControls(central);
	setupLayeredControls(central);

	// --- Main Buttons ---
	m_exportPNGBtn = new QPushButton("Export PNG", central);
	m_exportSVGBtn = new QPushButton("Export SVG", central);
	m_resetViewBtn = new QPushButton("Reset View", central);

	auto *controlPanel = new QWidget(this);
	auto *controlLayout = new QVBoxLayout(controlPanel);
	controlLayout->addWidget(m_concentricGroup);
	controlLayout->addWidget(m_layeredGroup);
	controlLayout->addStretch();
	controlLayout->addWidget(m_resetViewBtn);
	controlLayout->addWidget(m_exportPNGBtn);
	controlLayout->addWidget(m_exportSVGBtn);

	// --- View ---
	m_view->setRenderHint(QPainter::Antialiasing, true);
	m_view->setDragMode(QGraphicsView::ScrollHandDrag);

	// --- Main Layout ---
	auto *mainLayout = new QHBoxLayout(central);
	mainLayout->addWidget(controlPanel);
	mainLayout->addWidget(m_view, 1);

	statusBar()->showMessage("Ready");
}

void MainWindow::setupConcentricControls(QWidget *parent)
{
	m_concentricGroup = new QGroupBox("Concentric Mode", parent);
	m_concentricGroup->setCheckable(true);

	m_nSidesEdit = new QLineEdit(m_concentricGroup);
	m_nSidesEdit->setPlaceholderText("e.g., 48, 32, 24, 16, 8");
	m_nSidesEdit->setText("48, 32, 24, 16, 8");

	m_radiiEdit = new QLineEdit(m_concentricGroup);
	m_radiiEdit->setPlaceholderText("e.g., 20, 180, 140, 100, 60");
	m_radiiEdit->setText("20, 180, 140, 100, 60");

	m_ringJitterSpin = new QDoubleSpinBox(m_concentricGroup);
	m_ringJitterSpin->setRange(0.0, 45.0);
	m_ringJitterSpin->setValue(0.0);
	m_ringJitterSpin->setSuffix(" deg");

	m_connectTwoCheck = new QCheckBox("Connect two nearest", m_concentricGroup);
	m_connectTwoCheck->setChecked(true);

	m_connectNodesInRoundCheck = new QCheckBox("Connect nodes in round", m_concentricGroup);
	m_connectNodesInRoundCheck->setChecked(false);

	auto *layout = new QFormLayout(m_concentricGroup);
	layout->addRow("Sides per Ring:", m_nSidesEdit);
	layout->addRow("Radii per Ring:", m_radiiEdit);
	layout->addRow("Ring Jitter:", m_ringJitterSpin);
	layout->addRow(m_connectTwoCheck);
	layout->addRow(m_connectNodesInRoundCheck);
}

void MainWindow::setupLayeredControls(QWidget *parent)
{
	m_layeredGroup = new QGroupBox("Layered Polygon Mode", parent);
	m_layeredGroup->setCheckable(true);

	m_numLayersSpin = new QSpinBox(m_layeredGroup);
	m_numLayersSpin->setRange(1, 10);
	m_numLayersSpin->setValue(3);

	m_layerNSidesEdit = new QLineEdit(m_layeredGroup);
	m_layerNSidesEdit->setPlaceholderText("e.g., 30, 10, 5");
	m_layerNSidesEdit->setText("30, 10, 5");

	m_layerRadiiEdit = new QLineEdit(m_layeredGroup);
	m_layerRadiiEdit->setPlaceholderText("e.g., 240, 160, 90");
	m_layerRadiiEdit->setText("240, 160, 90");

	m_layerPhaseDegEdit = new QLineEdit(m_layeredGroup);
	m_layerPhaseDegEdit->setPlaceholderText("e.g., 0, 36, 0");
	m_layerPhaseDegEdit->setText("0, 36, 0");

	m_layerSubdivPerEdgeEdit = new QLineEdit(m_layeredGroup);
	m_layerSubdivPerEdgeEdit->setPlaceholderText("e.g., 1, 0, 0");
	m_layerSubdivPerEdgeEdit->setText("1, 0, 0");

	m_layerSubdivOffsetEdit = new QLineEdit(m_layeredGroup);
	m_layerSubdivOffsetEdit->setPlaceholderText("e.g., 0.5, 0, 0");
	m_layerSubdivOffsetEdit->setText("0.5, 0, 0");

	m_layerDrawRimEdit = new QLineEdit(m_layeredGroup);
	m_layerDrawRimEdit->setPlaceholderText("e.g., 1, 1, 0 (1=true, 0=false)");
	m_layerDrawRimEdit->setText("1, 1, 0");

	m_layerDrawDiagonalsEdit = new QLineEdit(m_layeredGroup);
	m_layerDrawDiagonalsEdit->setPlaceholderText("e.g., 0, 1, 0");
	m_layerDrawDiagonalsEdit->setText("0, 1, 0");

	m_layerSkipKEdit = new QLineEdit(m_layeredGroup);
	m_layerSkipKEdit->setPlaceholderText("e.g., 0, 2, 0");
	m_layerSkipKEdit->setText("0, 2, 0");

	m_interLayerConnectOneToOneEdit = new QLineEdit(m_layeredGroup);
	m_interLayerConnectOneToOneEdit->setPlaceholderText("e.g., 0, 0");
	m_interLayerConnectOneToOneEdit->setText("0, 0");

	m_interLayerConnectOneToTwoEdit = new QLineEdit(m_layeredGroup);
	m_interLayerConnectOneToTwoEdit->setPlaceholderText("e.g., 1, 1");
	m_interLayerConnectOneToTwoEdit->setText("1, 1");

	m_interLayerConnectZigzagEdit = new QLineEdit(m_layeredGroup);
	m_interLayerConnectZigzagEdit->setPlaceholderText("e.g., 1, 0");
	m_interLayerConnectZigzagEdit->setText("1, 0");

	m_jitterDegSpin = new QDoubleSpinBox(m_layeredGroup);
	m_jitterDegSpin->setRange(0.0, 45.0);
	m_jitterDegSpin->setValue(0.0);
	m_jitterDegSpin->setSuffix(" deg");

	m_seedSpin = new QSpinBox(m_layeredGroup);
	m_seedSpin->setRange(0, 999999);
	m_seedSpin->setValue(42);

	m_connectCenterCheck = new QCheckBox("Connect to Center", m_layeredGroup);
	m_connectCenterCheck->setChecked(true);

	auto *layout = new QFormLayout(m_layeredGroup);
	layout->addRow("Num Layers:", m_numLayersSpin);
	layout->addRow("Layer N Sides:", m_layerNSidesEdit);
	layout->addRow("Layer Radii:", m_layerRadiiEdit);
	layout->addRow("Layer Phase Deg:", m_layerPhaseDegEdit);
	layout->addRow("Layer Subdiv Per Edge:", m_layerSubdivPerEdgeEdit);
	layout->addRow("Layer Subdiv Offset:", m_layerSubdivOffsetEdit);
	layout->addRow("Layer Draw Rim:", m_layerDrawRimEdit);
	layout->addRow("Layer Draw Diagonals:", m_layerDrawDiagonalsEdit);
	layout->addRow("Layer Skip K:", m_layerSkipKEdit);
	layout->addRow("Inter-Layer 1-1:", m_interLayerConnectOneToOneEdit);
	layout->addRow("Inter-Layer 1-2:", m_interLayerConnectOneToTwoEdit);
	layout->addRow("Inter-Layer Zigzag:", m_interLayerConnectZigzagEdit);
	layout->addRow("Jitter Deg:", m_jitterDegSpin);
	layout->addRow("Seed:", m_seedSpin);
	layout->addRow(m_connectCenterCheck);
}

void MainWindow::connectSignals()
{
	// Connect concentric controls
	connect(m_concentricGroup, &QGroupBox::toggled, this, &MainWindow::onConcentricToggled);
	connect(m_nSidesEdit, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
	connect(m_radiiEdit, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
	connect(m_ringJitterSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &MainWindow::onParamsChanged);
	connect(m_connectTwoCheck, &QCheckBox::toggled, this, &MainWindow::onParamsChanged);
	connect(m_connectNodesInRoundCheck, &QCheckBox::toggled, this, &MainWindow::onParamsChanged);

	// Connect layered polygon controls
	connect(m_layeredGroup, &QGroupBox::toggled, this, &MainWindow::onLayeredToggled);
	connect(m_numLayersSpin, qOverload<int>(&QSpinBox::valueChanged), this, &MainWindow::onParamsChanged);
	connect(m_layerNSidesEdit, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
	connect(m_layerRadiiEdit, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
	connect(m_layerPhaseDegEdit, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
	connect(m_layerSubdivPerEdgeEdit, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
	connect(m_layerSubdivOffsetEdit, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
	connect(m_layerDrawRimEdit, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
	connect(m_layerDrawDiagonalsEdit, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
	connect(m_layerSkipKEdit, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
	connect(m_interLayerConnectOneToOneEdit, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
	connect(m_interLayerConnectOneToTwoEdit, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
	connect(m_interLayerConnectZigzagEdit, &QLineEdit::textChanged, this, &MainWindow::onParamsChanged);
	connect(m_jitterDegSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &MainWindow::onParamsChanged);
	connect(m_seedSpin, qOverload<int>(&QSpinBox::valueChanged), this, &MainWindow::onParamsChanged);
	connect(m_connectCenterCheck, &QCheckBox::toggled, this, &MainWindow::onParamsChanged);

	// Connect main buttons
	connect(m_exportPNGBtn, &QPushButton::clicked, this, &MainWindow::onExportPNG);
	connect(m_exportSVGBtn, &QPushButton::clicked, this, &MainWindow::onExportSVG);
	connect(m_resetViewBtn, &QPushButton::clicked, this, &MainWindow::onResetView);
}

Params MainWindow::collectParams()
{
	Params p;
	if (m_layeredGroup->isChecked())
	{
		p.layeredPolygonMode = true;
		p.concentricMode = false;
		ParamsLayered lp;
		lp.L = m_numLayersSpin->value();
		lp.jitterDeg = m_jitterDegSpin->value();
		lp.seed = m_seedSpin->value();
		lp.connectCenter = m_connectCenterCheck->isChecked();

		std::vector<int> nSides = parseInts(m_layerNSidesEdit->text());
		std::vector<double> radii = parseDoubles(m_layerRadiiEdit->text());
		std::vector<double> phaseDegs = parseDoubles(m_layerPhaseDegEdit->text());
		std::vector<int> subdivPerEdges = parseInts(m_layerSubdivPerEdgeEdit->text());
		std::vector<double> subdivOffsets = parseDoubles(m_layerSubdivOffsetEdit->text());
		std::vector<int> drawRims = parseInts(m_layerDrawRimEdit->text());
		std::vector<int> drawDiagonals = parseInts(m_layerDrawDiagonalsEdit->text());
		std::vector<int> skipKs = parseInts(m_layerSkipKEdit->text());

		lp.layers.resize(lp.L);
		for (int i = 0; i < lp.L; ++i)
		{
			if (i < nSides.size())
				lp.layers[i].n = nSides[i];
			if (i < radii.size())
				lp.layers[i].radius = radii[i];
			if (i < phaseDegs.size())
				lp.layers[i].phaseDeg = phaseDegs[i];
			if (i < subdivPerEdges.size())
				lp.layers[i].subdivPerEdge = subdivPerEdges[i];
			if (i < subdivOffsets.size())
				lp.layers[i].subdivOffset = subdivOffsets[i];
			if (i < drawRims.size())
				lp.layers[i].drawRim = (bool)drawRims[i];
			if (i < drawDiagonals.size())
				lp.layers[i].drawDiagonals = (bool)drawDiagonals[i];
			if (i < skipKs.size())
				lp.layers[i].skipK = skipKs[i];
		}

		std::vector<int> connectOneToOnes = parseInts(m_interLayerConnectOneToOneEdit->text());
		std::vector<int> connectOneToTwos = parseInts(m_interLayerConnectOneToTwoEdit->text());
		std::vector<int> connectZigzags = parseInts(m_interLayerConnectZigzagEdit->text());

		lp.between.resize(lp.L - 1);
		for (int i = 0; i < lp.L - 1; ++i)
		{
			if (i < connectOneToOnes.size())
				lp.between[i].connectOneToOne = (bool)connectOneToOnes[i];
			if (i < connectOneToTwos.size())
				lp.between[i].connectOneToTwo = (bool)connectOneToTwos[i];
			if (i < connectZigzags.size())
				lp.between[i].connectZigzag = (bool)connectZigzags[i];
		}
		p.layeredParams = lp;
	}
	else
	{ // Default to concentric
		p.concentricMode = true;
		p.layeredPolygonMode = false;
		p.nSides = parseInts(m_nSidesEdit->text());
		p.r = parseDoubles(m_radiiEdit->text());
		p.ringJitterDeg = m_ringJitterSpin->value();
		p.connectTwo = m_connectTwoCheck->isChecked();
		p.useRim = m_connectNodesInRoundCheck->isChecked();
		p.seed = QDateTime::currentMSecsSinceEpoch(); // Or a fixed seed from UI if added
	}
	return p;
}

void MainWindow::onConcentricToggled(bool checked)
{
	if (checked)
	{
		m_layeredGroup->setChecked(false);
	}
	onParamsChanged();
}

void MainWindow::onLayeredToggled(bool checked)
{
	if (checked)
	{
		m_concentricGroup->setChecked(false);
	}
	onParamsChanged();
}

void MainWindow::onParamsChanged()
{
	Params params = collectParams();

	if (params.layeredPolygonMode)
	{
		m_scene->updatePreview(params.layeredParams);
	}
	else
	{
		m_scene->updatePreview(params);
	}
	statusBar()->showMessage("Parameters updated");
}

void MainWindow::onExportPNG()
{
	const QString fileName = QFileDialog::getSaveFileName(this, "保存为PNG", "topology.png", "PNG (*.png)");
	if (fileName.isEmpty())
		return;

	// Use the view's visible rectangle as the source
	const QRectF sourceRect = m_view->mapToScene(m_view->viewport()->rect()).boundingRect();

	// Create an image and painter
	QImage image(sourceRect.size().toSize(), QImage::Format_ARGB32_Premultiplied);
	image.fill(Qt::white); // Use a white background

	QPainter painter(&image);
	painter.setRenderHint(QPainter::Antialiasing, true);

	// Render the scene
	m_scene->render(&painter, QRectF(), sourceRect);

	image.save(fileName);
	statusBar()->showMessage("已导出PNG: " + fileName, 3000);
}

void MainWindow::onExportSVG()
{
	const QString fileName = QFileDialog::getSaveFileName(this, "保存为SVG", "topology.svg", "SVG (*.svg)");
	if (fileName.isEmpty())
		return;

	// Use the view's visible rectangle as the source
	const QRectF sourceRect = m_view->mapToScene(m_view->viewport()->rect()).boundingRect();

	QSvgGenerator generator;
	generator.setFileName(fileName);
	generator.setSize(sourceRect.size().toSize());
	generator.setViewBox(sourceRect);

	QPainter painter(&generator);
	m_scene->render(&painter, sourceRect);

	statusBar()->showMessage("已导出SVG: " + fileName, 3000);
}

void MainWindow::onResetView()
{
	m_view->resetTransform();
	m_view->fitInView(m_scene->itemsBoundingRect().adjusted(-40, -40, 40, 40), Qt::KeepAspectRatio);
}