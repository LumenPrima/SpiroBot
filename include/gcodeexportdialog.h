#ifndef GCODEEXPORTDIALOG_H
#define GCODEEXPORTDIALOG_H

#include <QDialog>
#include "gcodegenerator.h"

class QLineEdit;
class QDoubleSpinBox;
class QComboBox;
class QPlainTextEdit;

class GcodeExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GcodeExportDialog(QWidget *parent = nullptr);
    GcodeGenerator::Config getConfig() const;

private:
    void loadConfigurationFromFile();

    QDoubleSpinBox *drawingAreaWidthSpinBox;
    QDoubleSpinBox *drawingAreaHeightSpinBox;
    QDoubleSpinBox *maxSpeedSpinBox;
    QDoubleSpinBox *maxAccelerationSpinBox;
    QDoubleSpinBox *penUpPositionSpinBox;
    QDoubleSpinBox *penDownPositionSpinBox;
    QDoubleSpinBox *travelSpeedSpinBox;
    QDoubleSpinBox *drawingSpeedSpinBox;
    QComboBox *originComboBox;
    QPlainTextEdit *startGcodeEdit;
    QPlainTextEdit *endGcodeEdit;
};

#endif // GCODEEXPORTDIALOG_H