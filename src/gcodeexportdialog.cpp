#include "gcodeexportdialog.h"
#include "gcodegenerator.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QPlainTextEdit>

GcodeExportDialog::GcodeExportDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Gcode Export Settings"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    // Drawing Area Width
    drawingAreaWidthSpinBox = new QDoubleSpinBox(this);
    drawingAreaWidthSpinBox->setRange(1, 1000);
    drawingAreaWidthSpinBox->setValue(200);
    drawingAreaWidthSpinBox->setSuffix(" mm");
    formLayout->addRow(tr("Drawing Area Width:"), drawingAreaWidthSpinBox);

    // Drawing Area Height
    drawingAreaHeightSpinBox = new QDoubleSpinBox(this);
    drawingAreaHeightSpinBox->setRange(1, 1000);
    drawingAreaHeightSpinBox->setValue(200);
    drawingAreaHeightSpinBox->setSuffix(" mm");
    formLayout->addRow(tr("Drawing Area Height:"), drawingAreaHeightSpinBox);

    // Max Speed
    maxSpeedSpinBox = new QDoubleSpinBox(this);
    maxSpeedSpinBox->setRange(1, 10000);
    maxSpeedSpinBox->setValue(3000);
    maxSpeedSpinBox->setSuffix(" mm/min");
    formLayout->addRow(tr("Max Speed:"), maxSpeedSpinBox);

    // Max Acceleration
    maxAccelerationSpinBox = new QDoubleSpinBox(this);
    maxAccelerationSpinBox->setRange(1, 10000);
    maxAccelerationSpinBox->setValue(500);
    maxAccelerationSpinBox->setSuffix(" mm/s²");
    formLayout->addRow(tr("Max Acceleration:"), maxAccelerationSpinBox);

    // Pen Up Position
    penUpPositionSpinBox = new QDoubleSpinBox(this);
    penUpPositionSpinBox->setRange(-50, 50);
    penUpPositionSpinBox->setValue(5);
    penUpPositionSpinBox->setSuffix(" mm");
    formLayout->addRow(tr("Pen Up Position:"), penUpPositionSpinBox);

    // Pen Down Position
    penDownPositionSpinBox = new QDoubleSpinBox(this);
    penDownPositionSpinBox->setRange(-50, 50);
    penDownPositionSpinBox->setValue(-1);
    penDownPositionSpinBox->setSuffix(" mm");
    formLayout->addRow(tr("Pen Down Position:"), penDownPositionSpinBox);

    // Travel Speed
    travelSpeedSpinBox = new QDoubleSpinBox(this);
    travelSpeedSpinBox->setRange(1, 10000);
    travelSpeedSpinBox->setValue(3000);
    travelSpeedSpinBox->setSuffix(" mm/min");
    formLayout->addRow(tr("Travel Speed:"), travelSpeedSpinBox);

    // Drawing Speed
    drawingSpeedSpinBox = new QDoubleSpinBox(this);
    drawingSpeedSpinBox->setRange(1, 10000);
    drawingSpeedSpinBox->setValue(1500);
    drawingSpeedSpinBox->setSuffix(" mm/min");
    formLayout->addRow(tr("Drawing Speed:"), drawingSpeedSpinBox);

    // Origin selection
    originComboBox = new QComboBox(this);
    originComboBox->addItem(tr("Bottom Left"), static_cast<int>(GcodeGenerator::Origin::BottomLeft));
    originComboBox->addItem(tr("Top Left"), static_cast<int>(GcodeGenerator::Origin::TopLeft));
    originComboBox->addItem(tr("Top Right"), static_cast<int>(GcodeGenerator::Origin::TopRight));
    originComboBox->addItem(tr("Bottom Right"), static_cast<int>(GcodeGenerator::Origin::BottomRight));
    originComboBox->addItem(tr("Center"), static_cast<int>(GcodeGenerator::Origin::Center));
    formLayout->addRow(tr("Origin:"), originComboBox);

    mainLayout->addLayout(formLayout);

    // Start Gcode
    QLabel *startGcodeLabel = new QLabel(tr("Start Gcode:"), this);
    startGcodeEdit = new QPlainTextEdit(this);
    startGcodeEdit->setPlainText("G21 ; Set units to millimeters\n"
                                 "G90 ; Use absolute coordinates\n"
                                 "G92 X0 Y0 Z0 ; Set current position as home\n"
                                 "M5 ; Ensure spindle is off");
    mainLayout->addWidget(startGcodeLabel);
    mainLayout->addWidget(startGcodeEdit);

    // End Gcode
    QLabel *endGcodeLabel = new QLabel(tr("End Gcode:"), this);
    endGcodeEdit = new QPlainTextEdit(this);
    endGcodeEdit->setPlainText("G0 X0 Y0 ; Return to home position\n"
                               "M5 ; Ensure spindle is off\n"
                               "M2 ; End program");
    mainLayout->addWidget(endGcodeLabel);
    mainLayout->addWidget(endGcodeEdit);

    // Add some vertical spacing
    mainLayout->addSpacing(20);

    // Create and add the button box
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
}

GcodeGenerator::Config GcodeExportDialog::getConfig() const
{
    GcodeGenerator::Config config;
    config.drawingAreaWidth = drawingAreaWidthSpinBox->value();
    config.drawingAreaHeight = drawingAreaHeightSpinBox->value();
    config.maxSpeed = maxSpeedSpinBox->value();
    config.maxAcceleration = maxAccelerationSpinBox->value();
    config.penUpPosition = penUpPositionSpinBox->value();
    config.penDownPosition = penDownPositionSpinBox->value();
    config.travelSpeed = travelSpeedSpinBox->value();
    config.drawingSpeed = drawingSpeedSpinBox->value();
    config.origin = static_cast<GcodeGenerator::Origin>(originComboBox->currentData().toInt());
    config.startGcode = startGcodeEdit->toPlainText();
    config.endGcode = endGcodeEdit->toPlainText();
    return config;
}