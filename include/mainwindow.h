#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSlider>
#include <QSpinBox>
#include <QLabel>
#include <QDoubleSpinBox>

class DrawingArea;
class GcodeGenerator;  // Forward declaration

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateSpirograph();
    void exportToSVG();
    void exportToPNG();
    void exportToGcode();
    void updateAnalysis();
    void updateValueLabels();

private:
    void setupUI();

    DrawingArea *drawingArea;
    QSlider *outerRadiusSlider;
    QSlider *innerRadiusSlider;
    QSlider *penOffsetSlider;
    QSpinBox *rotationsSpinBox;
    QDoubleSpinBox *lineThicknessSpinBox;
    QSpinBox *numPensSpinBox;
    QDoubleSpinBox *rotationOffsetSpinBox;
    QLabel *statusLabel;
    QLabel *pathLengthLabel;
    QLabel *outerRadiusValueLabel;
    QLabel *innerRadiusValueLabel;
    QLabel *penOffsetValueLabel;
    //QLabel *rotationsValueLabel;
    //QLabel *lineThicknessValueLabel;
    //QLabel *numPensValueLabel;
    //QLabel *rotationOffsetValueLabel;
};

#endif // MAINWINDOW_H