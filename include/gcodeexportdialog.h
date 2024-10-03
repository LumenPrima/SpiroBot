// In a new file, gcodeexportdialog.h
#ifndef GCODEEXPORTDIALOG_H
#define GCODEEXPORTDIALOG_H

#include <QDialog>
#include "gcodegenerator.h"

class QLineEdit;
class QDoubleSpinBox;

class GcodeExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GcodeExportDialog(QWidget *parent = nullptr);
    GcodeGenerator::Config getConfig() const;

private:
    QDoubleSpinBox *drawingAreaWidthSpinBox;
    QDoubleSpinBox *drawingAreaHeightSpinBox;
    QDoubleSpinBox *maxSpeedSpinBox;
    QDoubleSpinBox *maxAccelerationSpinBox;
    QDoubleSpinBox *penUpPositionSpinBox;
    QDoubleSpinBox *penDownPositionSpinBox;
    QDoubleSpinBox *travelSpeedSpinBox;
    QDoubleSpinBox *drawingSpeedSpinBox;
};


#endif // GCODEEXPORTDIALOG_H