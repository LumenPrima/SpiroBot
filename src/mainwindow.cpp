#include "mainwindow.h"
#include "drawingarea.h"
#include "gcodeexportdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("SpiroBot");
    resize(1400, 1200);
    setupUI();
}

MainWindow::~MainWindow()
{
}


void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Drawing area
    drawingArea = new DrawingArea(this);
    mainLayout->addWidget(drawingArea, 1);

    // Controls
    QGroupBox *controlsGroup = new QGroupBox("Controls", this);
    QVBoxLayout *controlsLayout = new QVBoxLayout(controlsGroup);

    // Outer Radius
    QHBoxLayout *outerRadiusLayout = new QHBoxLayout();
    outerRadiusSlider = new QSlider(Qt::Horizontal, this);
    outerRadiusSlider->setRange(50, 200);
    outerRadiusValueLabel = new QLabel(this);
    outerRadiusLayout->addWidget(new QLabel("Outer Radius:"));
    outerRadiusLayout->addWidget(outerRadiusSlider);
    outerRadiusLayout->addWidget(outerRadiusValueLabel);
    controlsLayout->addLayout(outerRadiusLayout);

    // Inner Radius
    QHBoxLayout *innerRadiusLayout = new QHBoxLayout();
    innerRadiusSlider = new QSlider(Qt::Horizontal, this);
    innerRadiusSlider->setRange(10, 100);
    innerRadiusValueLabel = new QLabel(this);
    innerRadiusLayout->addWidget(new QLabel("Inner Radius:"));
    innerRadiusLayout->addWidget(innerRadiusSlider);
    innerRadiusLayout->addWidget(innerRadiusValueLabel);
    controlsLayout->addLayout(innerRadiusLayout);

    // Pen Offset
    QHBoxLayout *penOffsetLayout = new QHBoxLayout();
    penOffsetSlider = new QSlider(Qt::Horizontal, this);
    penOffsetSlider->setRange(1, 100);
    penOffsetValueLabel = new QLabel(this);
    penOffsetLayout->addWidget(new QLabel("Pen Offset:"));
    penOffsetLayout->addWidget(penOffsetSlider);
    penOffsetLayout->addWidget(penOffsetValueLabel);
    controlsLayout->addLayout(penOffsetLayout);

    // Rotations
    QHBoxLayout *rotationsLayout = new QHBoxLayout();
    rotationsSpinBox = new QSpinBox(this);
    rotationsSpinBox->setRange(1, 100);
    rotationsLayout->addWidget(new QLabel("Rotations:"));
    rotationsLayout->addWidget(rotationsSpinBox);
    controlsLayout->addLayout(rotationsLayout);

    // Line Thickness
    QHBoxLayout *lineThicknessLayout = new QHBoxLayout();
    lineThicknessSpinBox = new QDoubleSpinBox(this);
    lineThicknessSpinBox->setRange(0.1, 5.0);
    lineThicknessSpinBox->setSingleStep(0.1);
    lineThicknessSpinBox->setValue(1.0);
    lineThicknessLayout->addWidget(new QLabel("Line Thickness:"));
    lineThicknessLayout->addWidget(lineThicknessSpinBox);
    controlsLayout->addLayout(lineThicknessLayout);

    // Number of Pens
    QHBoxLayout *numPensLayout = new QHBoxLayout();
    numPensSpinBox = new QSpinBox(this);
    numPensSpinBox->setRange(1, 5);
    numPensSpinBox->setValue(1);
    numPensLayout->addWidget(new QLabel("Number of Pens:"));
    numPensLayout->addWidget(numPensSpinBox);
    controlsLayout->addLayout(numPensLayout);

    // Rotation Offset
    QHBoxLayout *rotationOffsetLayout = new QHBoxLayout();
    rotationOffsetSpinBox = new QDoubleSpinBox(this);
    rotationOffsetSpinBox->setRange(0, 360);
    rotationOffsetSpinBox->setSingleStep(15);
    rotationOffsetLayout->addWidget(new QLabel("Rotation Offset (degrees):"));
    rotationOffsetLayout->addWidget(rotationOffsetSpinBox);
    controlsLayout->addLayout(rotationOffsetLayout);

    pathLengthLabel = new QLabel("Path Length: N/A", this);
    controlsLayout->addWidget(pathLengthLabel);

    statusLabel = new QLabel("Ready", this);
    controlsLayout->addWidget(statusLabel);

    controlsLayout->addStretch(1);

    mainLayout->addWidget(controlsGroup);

    setCentralWidget(centralWidget);

    // Connect signals and slots
    connect(outerRadiusSlider, &QSlider::valueChanged, this, &MainWindow::updateSpirograph);
    connect(innerRadiusSlider, &QSlider::valueChanged, this, &MainWindow::updateSpirograph);
    connect(penOffsetSlider, &QSlider::valueChanged, this, &MainWindow::updateSpirograph);
    connect(rotationsSpinBox, &QSpinBox::valueChanged, this, &MainWindow::updateSpirograph);
    connect(lineThicknessSpinBox, &QDoubleSpinBox::valueChanged, this, &MainWindow::updateSpirograph);
    connect(numPensSpinBox, &QSpinBox::valueChanged, this, &MainWindow::updateSpirograph);
    connect(rotationOffsetSpinBox, &QDoubleSpinBox::valueChanged, this, &MainWindow::updateSpirograph);

    connect(drawingArea, &DrawingArea::spirographUpdated, this, &MainWindow::updateAnalysis);

    // Connect value change signals to updateValueLabels
    connect(outerRadiusSlider, &QSlider::valueChanged, this, &MainWindow::updateValueLabels);
    connect(innerRadiusSlider, &QSlider::valueChanged, this, &MainWindow::updateValueLabels);
    connect(penOffsetSlider, &QSlider::valueChanged, this, &MainWindow::updateValueLabels);
    connect(rotationsSpinBox, &QSpinBox::valueChanged, this, &MainWindow::updateValueLabels);
    connect(lineThicknessSpinBox, &QDoubleSpinBox::valueChanged, this, &MainWindow::updateValueLabels);
    connect(numPensSpinBox, &QSpinBox::valueChanged, this, &MainWindow::updateValueLabels);
    connect(rotationOffsetSpinBox, &QDoubleSpinBox::valueChanged, this, &MainWindow::updateValueLabels);

    QMenu *exportMenu = menuBar()->addMenu(tr("&Export"));
    QAction *exportSVGAction = new QAction(tr("Export as &SVG"), this);
    connect(exportSVGAction, &QAction::triggered, this, &MainWindow::exportToSVG);
    exportMenu->addAction(exportSVGAction);

    QAction *exportPNGAction = new QAction(tr("Export as &PNG"), this);
    connect(exportPNGAction, &QAction::triggered, this, &MainWindow::exportToPNG);
    exportMenu->addAction(exportPNGAction);

    QAction *exportGcodeAction = new QAction(tr("Export as &Gcode"), this);
    connect(exportGcodeAction, &QAction::triggered, this, &MainWindow::exportToGcode);
    exportMenu->addAction(exportGcodeAction);

    // Initial update
    updateValueLabels();
    updateSpirograph();
}


void MainWindow::updateSpirograph()
{
    drawingArea->setParameters(
        outerRadiusSlider->value(),
        innerRadiusSlider->value(),
        penOffsetSlider->value(),
        rotationsSpinBox->value(),
        lineThicknessSpinBox->value(),
        numPensSpinBox->value(),
        rotationOffsetSpinBox->value()
    );
    drawingArea->generateSpirograph();
    drawingArea->update();
    statusLabel->setText("Spirograph updated");
}

void MainWindow::exportToSVG()
{
    QString filename = QFileDialog::getSaveFileName(this, 
        tr("Export SVG"), "", tr("SVG Files (*.svg)"));
    if (filename.isEmpty())
        return;

    if (!filename.endsWith(".svg", Qt::CaseInsensitive))
        filename += ".svg";

    if (drawingArea->exportToSVG(filename)) {
        statusLabel->setText("SVG exported successfully");
    } else {
        QMessageBox::critical(this, tr("Export Failed"),
            tr("Failed to export the SVG file."));
    }
}

void MainWindow::exportToPNG()
{
    QString filename = QFileDialog::getSaveFileName(this, 
        tr("Export PNG"), "", tr("PNG Files (*.png)"));
    if (filename.isEmpty())
        return;

    if (!filename.endsWith(".png", Qt::CaseInsensitive))
        filename += ".png";

    bool ok;
    int width = QInputDialog::getInt(this, tr("PNG Width"),
                                     tr("Enter PNG width:"), drawingArea->width(), 1, 10000, 1, &ok);
    if (!ok) return;

    int height = QInputDialog::getInt(this, tr("PNG Height"),
                                      tr("Enter PNG height:"), drawingArea->height(), 1, 10000, 1, &ok);
    if (!ok) return;

    if (drawingArea->exportToPNG(filename, width, height)) {
        statusLabel->setText("PNG exported successfully");
    } else {
        QMessageBox::critical(this, tr("Export Failed"),
            tr("Failed to export the PNG file."));
    }
}

void MainWindow::exportToGcode()
{
    QString filename = QFileDialog::getSaveFileName(this, 
        tr("Export Gcode"), "", tr("Gcode Files (*.gcode)"));
    if (filename.isEmpty())
        return;

    if (!filename.endsWith(".gcode", Qt::CaseInsensitive))
        filename += ".gcode";

    GcodeExportDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        GcodeGenerator::Config config = dialog.getConfig();
        if (drawingArea->exportToGcode(filename, &config)) {  // Pass the address of config
            statusLabel->setText("Gcode exported successfully");
        } else {
            QMessageBox::critical(this, tr("Export Failed"),
                tr("Failed to export the Gcode file."));
        }
    }
}

void MainWindow::updateAnalysis()
{
    double pathLength = drawingArea->calculateTotalPathLength();
    pathLengthLabel->setText(QString("Path Length: %1").arg(pathLength, 0, 'f', 2));
}

void MainWindow::updateValueLabels()
{
    outerRadiusValueLabel->setText(QString::number(outerRadiusSlider->value()));
    innerRadiusValueLabel->setText(QString::number(innerRadiusSlider->value()));
    penOffsetValueLabel->setText(QString::number(penOffsetSlider->value()));
}