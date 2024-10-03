#include "gcodeexportdialog.h"
#include "gcodegenerator.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>

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
    penUpPositionSpinBox->setRange(0, 50);
    penUpPositionSpinBox->setValue(5);
    penUpPositionSpinBox->setSuffix(" mm");
    formLayout->addRow(tr("Pen Up Position:"), penUpPositionSpinBox);

    // Pen Down Position
    penDownPositionSpinBox = new QDoubleSpinBox(this);
    penDownPositionSpinBox->setRange(-50, 0);
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

    mainLayout->addLayout(formLayout);

    // Add some vertical spacing
    mainLayout->addSpacing(20);

    // Add a label with some usage instructions
    QLabel *instructionLabel = new QLabel(tr("Adjust the settings above to configure the Gcode export. "
                                             "Make sure the values are compatible with your FluidNC setup."), this);
    instructionLabel->setWordWrap(true);
    mainLayout->addWidget(instructionLabel);

    // Add some more vertical spacing
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
    return config;
}