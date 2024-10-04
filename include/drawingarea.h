#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include <QWidget>
#include <QPainterPath>
#include <QColor>
#include <QTimer>
#include "gcodegenerator.h" // Add this line to include the full definition of GcodeGenerator

class DrawingArea : public QWidget
{
    Q_OBJECT

public:
    explicit DrawingArea(QWidget *parent = nullptr);
    ~DrawingArea();

    void setParameters(int outerRadius, int innerRadius, int penOffset, int rotations,
                       double lineThickness, int numPens, double rotationOffset);
    void generateSpirograph();
    void generateSpirographStep(int step);
    bool exportToSVG(const QString &filename) const;
    bool exportToPNG(const QString &filename, int width = 0, int height = 0) const;
    bool exportToGcode(const QString &filename, const GcodeGenerator::Config& config) const;

    double calculateTotalPathLength() const;

    // New methods for gear visualization
    void startAnimation();
    void stopAnimation();

signals:
    void spirographUpdated();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateAnimation();

private:
    int outerRadius;
    int innerRadius;
    int penOffset;
    int rotations;
    double lineThickness;
    int numPens;
    double rotationOffset;
    QVector<QPainterPath> spirographPaths;
    QVector<QColor> penColors;

    QRectF boundingBox;
    double zoomFactor;

    // New members for gear visualization
    QTimer *animationTimer;
    double currentAngle;
    bool isAnimating;

    void generatePenColors();
    void calculateBoundingBoxAndZoom();
    
    // New methods for gear visualization
    void drawGears(QPainter &painter);
    void drawGear(QPainter &painter, const QPointF &center, double radius, const QColor &color);
    void drawPenHoles(QPainter &painter);
    QPointF calculatePenPosition();
    
    class DrawingAreaPrivate;
    DrawingAreaPrivate* d_ptr;
};

#endif // DRAWINGAREA_H