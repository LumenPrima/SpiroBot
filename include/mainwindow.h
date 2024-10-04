#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSlider>
#include <QSpinBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QTimer>

class DrawingArea;

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
    void updateAnimation();
    void on_closeLoopButton_clicked();
    void on_animateButton_clicked();

private:
    void setupUI();
    int calculateRotationsToCloseLoop(int outerRadius, int innerRadius);

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
    QPushButton *closeLoopButton;
    QPushButton *animateButton;
    QTimer *animationTimer;
    int currentStep;
    int totalRotations;
};

#endif // MAINWINDOW_H