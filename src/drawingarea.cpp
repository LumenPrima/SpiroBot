#include "drawingarea.h"
#include "gcodegenerator.h"
#include <QPainter>
#include <cmath>
#include <QSvgGenerator>
#include <QImage>
#include <QFile>
#include <QTextStream>
#include <QLineF>
#include <QtMath>



class DrawingArea::DrawingAreaPrivate
{
public:
    GcodeGenerator* gcodeGenerator;

    DrawingAreaPrivate() : gcodeGenerator(new GcodeGenerator()) {}
    ~DrawingAreaPrivate() { delete gcodeGenerator; }
};

DrawingArea::DrawingArea(QWidget *parent)
    : QWidget(parent), outerRadius(100), innerRadius(50), penOffset(25), rotations(5),
      lineThickness(1.0), numPens(1), rotationOffset(0), d_ptr(new DrawingAreaPrivate())
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    generatePenColors();
}

DrawingArea::~DrawingArea()
{
    delete d_ptr;
}

void DrawingArea::setParameters(int outerRadius, int innerRadius, int penOffset, int rotations,
                                double lineThickness, int numPens, double rotationOffset)
{
    this->outerRadius = outerRadius;
    this->innerRadius = innerRadius;
    this->penOffset = penOffset;
    this->rotations = rotations;
    this->lineThickness = lineThickness;
    this->numPens = numPens;
    this->rotationOffset = rotationOffset;

    if (this->numPens != penColors.size()) {
        generatePenColors();
    }
}

void DrawingArea::generateSpirograph()
{
    spirographPaths.clear();
    spirographPaths.resize(numPens);

    double outerRadiusD = static_cast<double>(outerRadius);
    double innerRadiusD = static_cast<double>(innerRadius);
    double penOffsetD = static_cast<double>(penOffset);
    double rotationOffsetRad = rotationOffset * M_PI / 180.0;

    for (int pen = 0; pen < numPens; ++pen) {
        QPainterPath &path = spirographPaths[pen];
        path = QPainterPath();

        double t = 0.0;
        double stepSize = 0.01;
        int maxSteps = static_cast<int>(2 * M_PI / stepSize) * rotations;
        
        double penAngleOffset = 2 * M_PI * pen / numPens + rotationOffsetRad;

        for (int i = 0; i <= maxSteps; ++i) {
            double x = (outerRadiusD - innerRadiusD) * qCos(t) + 
                       penOffsetD * qCos(((outerRadiusD - innerRadiusD) * t / innerRadiusD) + penAngleOffset);
            double y = (outerRadiusD - innerRadiusD) * qSin(t) - 
                       penOffsetD * qSin(((outerRadiusD - innerRadiusD) * t / innerRadiusD) + penAngleOffset);

            if (i == 0) {
                path.moveTo(x, y);
            } else {
                path.lineTo(x, y);
            }

            t += stepSize;
        }
    }

    calculateBoundingBoxAndZoom();
    update();
    emit spirographUpdated();
}


void DrawingArea::generateSpirographStep(int step)
{
    spirographPaths.clear();
    spirographPaths.resize(numPens);

    double outerRadiusD = static_cast<double>(outerRadius);
    double innerRadiusD = static_cast<double>(innerRadius);
    double penOffsetD = static_cast<double>(penOffset);
    double rotationOffsetRad = rotationOffset * M_PI / 180.0;

    for (int pen = 0; pen < numPens; ++pen) {
        QPainterPath &path = spirographPaths[pen];
        path = QPainterPath();

        double t = 0.0;
        double stepSize = 0.01;
        int maxSteps = static_cast<int>(2 * M_PI / stepSize) * step;
        
        double penAngleOffset = 2 * M_PI * pen / numPens + rotationOffsetRad;

        for (int i = 0; i <= maxSteps; ++i) {
            double x = (outerRadiusD - innerRadiusD) * qCos(t) + 
                       penOffsetD * qCos((outerRadiusD - innerRadiusD) * t / innerRadiusD + penAngleOffset);
            double y = (outerRadiusD - innerRadiusD) * qSin(t) - 
                       penOffsetD * qSin((outerRadiusD - innerRadiusD) * t / innerRadiusD + penAngleOffset);

            if (i == 0) {
                path.moveTo(x, y);
            } else {
                path.lineTo(x, y);
            }

            t += stepSize;
        }
    }

    calculateBoundingBoxAndZoom();
    update();
    emit spirographUpdated();
}

void DrawingArea::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Center the spirograph
    painter.translate(width() / 2, height() / 2);

    // Apply zoom
    painter.scale(zoomFactor, zoomFactor);

    // Center the spirograph within its bounding box
    painter.translate(-boundingBox.center());

    // Draw the spirograph
    for (int i = 0; i < spirographPaths.size(); ++i) {
        painter.setPen(QPen(penColors[i], lineThickness / zoomFactor));
        painter.drawPath(spirographPaths[i]);
    }
}

void DrawingArea::generatePenColors()
{
    penColors.clear();
    for (int i = 0; i < numPens; ++i) {
        penColors.append(QColor::fromHsv(i * 360 / numPens, 255, 255));
    }
}

bool DrawingArea::exportToSVG(const QString &filename) const
{
    QSvgGenerator generator;
    generator.setFileName(filename);
    generator.setSize(QSize(width(), height()));
    generator.setViewBox(QRect(0, 0, width(), height()));
    generator.setTitle("SpiroBot SVG Export");
    generator.setDescription("Spirograph pattern generated by SpiroBot");

    QPainter painter;
    if (!painter.begin(&generator)) {
        return false;  // Failed to start painting
    }

    // Set up the painter similar to the paintEvent
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.translate(width() / 2, height() / 2);
    double scale = std::min(width(), height()) / (2.0 * outerRadius);
    painter.scale(scale, scale);

    // Draw the spirograph
    for (int i = 0; i < spirographPaths.size(); ++i) {
        painter.setPen(QPen(penColors[i], lineThickness / scale));
        painter.drawPath(spirographPaths[i]);
    }

    painter.end();
    return true;
}

bool DrawingArea::exportToPNG(const QString &filename, int width, int height) const
{
    // If width or height is 0, use the current size
    if (width <= 0) width = this->width();
    if (height <= 0) height = this->height();

    QImage image(width, height, QImage::Format_ARGB32);
    image.fill(Qt::white);  // Fill with white background

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.translate(width / 2, height / 2);
    double scale = std::min(width, height) / (2.0 * outerRadius);
    painter.scale(scale, scale);

    // Draw the spirograph
    for (int i = 0; i < spirographPaths.size(); ++i) {
        painter.setPen(QPen(penColors[i], lineThickness / scale));
        painter.drawPath(spirographPaths[i]);
    }

    painter.end();

    return image.save(filename, "PNG");
}

bool DrawingArea::exportToGcode(const QString &filename, const void* config) const
{
    const GcodeGenerator::Config* gcodeConfig = static_cast<const GcodeGenerator::Config*>(config);
    return d_ptr->gcodeGenerator->generateGcode(spirographPaths, *gcodeConfig, filename);
}

double DrawingArea::calculateTotalPathLength() const
{
    double totalLength = 0.0;
    for (const auto& path : spirographPaths) {
        for (int i = 1; i < path.elementCount(); ++i) {
            QPointF p1 = path.elementAt(i - 1);
            QPointF p2 = path.elementAt(i);
            totalLength += QLineF(p1, p2).length();
        }
    }
    return totalLength;
}

void DrawingArea::calculateBoundingBoxAndZoom()
{
    if (spirographPaths.isEmpty()) {
        boundingBox = QRectF();
        zoomFactor = 1.0;
        return;
    }

    boundingBox = spirographPaths[0].boundingRect();
    for (int i = 1; i < spirographPaths.size(); ++i) {
        boundingBox = boundingBox.united(spirographPaths[i].boundingRect());
    }

    // Add a small margin (5% on each side)
    double margin = std::max(boundingBox.width(), boundingBox.height()) * 0.05;
    boundingBox.adjust(-margin, -margin, margin, margin);

    // Calculate zoom factor
    double widthRatio = width() / boundingBox.width();
    double heightRatio = height() / boundingBox.height();
    zoomFactor = std::min(widthRatio, heightRatio);
}
