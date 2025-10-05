#pragma once

#include <QMainWindow>
#include "../core/Params.h"
#include "../core/LayeredParams.h"

#include <QGroupBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>

class QGraphicsView;
class QComboBox;
class QPushButton;
class TopologyScene;
class TopologyView;


class MainWindow : public QMainWindow {
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onParamsChanged();
    void onConcentricToggled(bool checked);
    void onLayeredToggled(bool checked);
    void onExportPNG();
    void onExportSVG();
    void onResetView();

private:
    void buildUi();
    void connectSignals();
    void setupOriginalControls(QWidget* parent);
    void setupConcentricControls(QWidget* parent);
    void setupLayeredControls(QWidget* parent);
    Params collectParams();


private:
    TopologyScene *m_scene;
    TopologyView *m_view;

    // UI Controls
    QGroupBox *m_concentricGroup;
    QLineEdit *m_nSidesEdit;
    QLineEdit *m_radiiEdit;
    QDoubleSpinBox *m_ringJitterSpin;
    QCheckBox *m_connectTwoCheck;
    QCheckBox *m_connectNodesInRoundCheck;

    QGroupBox *m_layeredGroup;
    QSpinBox *m_numLayersSpin;
    QLineEdit *m_layerNSidesEdit;
    QLineEdit *m_layerRadiiEdit;
    QLineEdit *m_layerPhaseDegEdit;
    QLineEdit *m_layerSubdivPerEdgeEdit;
    QLineEdit *m_layerSubdivOffsetEdit;
    QLineEdit *m_layerDrawRimEdit;
    QLineEdit *m_layerDrawDiagonalsEdit;
    QLineEdit *m_layerSkipKEdit;
    QLineEdit *m_interLayerConnectOneToOneEdit;
    QLineEdit *m_interLayerConnectOneToTwoEdit;
    QLineEdit *m_interLayerConnectZigzagEdit;
    QDoubleSpinBox *m_jitterDegSpin;
    QSpinBox *m_seedSpin;
    QCheckBox *m_connectCenterCheck;

    QPushButton *m_exportPNGBtn;
    QPushButton *m_exportSVGBtn;
    QPushButton *m_resetViewBtn;
};


