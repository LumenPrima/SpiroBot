#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include <QWidget>
#include <QPainterPath>
#include <QColor>

// Forward declaration
class GcodeGenerator;

class DrawingArea : public QWidget
{
    Q_OBJECT

public:
    explicit DrawingArea(QWidget *parent = nullptr);
    ~DrawingArea();

    void setParameters(int outerRadius, int innerRadius, int penOffset, int rotations,
                       double lineThickness, int numPens, double rotationOffset);
    void generateSpirograph();
    bool exportToSVG(const QString &filename) const;
    bool exportToPNG(const QString &filename, int width = 0, int height = 0) const;
    bool exportToGcode(const QString &filename, const void* config) const;

    double calculateTotalPathLength() const;

signals:
    void spirographUpdated();

protected:
    void paintEvent(QPaintEvent *event) override;

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

    void generatePenColors();
    void calculateBoundingBoxAndZoom();
    
    // Pointer to implementation to hide GcodeGenerator details
    class DrawingAreaPrivate;
    DrawingAreaPrivate* d_ptr;
};

#endif // DRAWINGAREA_H