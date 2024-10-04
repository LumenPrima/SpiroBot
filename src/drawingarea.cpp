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
#include <iostream>
#include <QDebug>
#include <stdexcept>



class DrawingArea::DrawingAreaPrivate
{
public:
    GcodeGenerator* gcodeGenerator;

    DrawingAreaPrivate() : gcodeGenerator(new GcodeGenerator()) {}
    ~DrawingAreaPrivate() { delete gcodeGenerator; }
};

DrawingArea::DrawingArea(QWidget *parent)
    : QWidget(parent), outerRadius(100), innerRadius(50), penOffset(25), rotations(5),
      lineThickness(1.0), numPens(1), rotationOffset(0), currentAngle(0), isAnimating(false)
{
    std::cout << "DrawingArea constructor started" << std::endl;
    qDebug() << "DrawingArea constructor started";

    try {
        std::cout << "Setting background role" << std::endl;
        qDebug() << "Setting background role";
        setBackgroundRole(QPalette::Base);

        std::cout << "Setting auto fill background" << std::endl;
        qDebug() << "Setting auto fill background";
        setAutoFillBackground(true);

        std::cout << "Generating pen colors" << std::endl;
        qDebug() << "Generating pen colors";
        generatePenColors();

        std::cout << "Creating animation timer" << std::endl;
        qDebug() << "Creating animation timer";
        animationTimer = new QTimer(this);

        std::cout << "Connecting animation timer" << std::endl;
        qDebug() << "Connecting animation timer";
        connect(animationTimer, &QTimer::timeout, this, &DrawingArea::updateAnimation);

        std::cout << "Creating GcodeGenerator" << std::endl;
        qDebug() << "Creating GcodeGenerator";
        d_ptr = new DrawingAreaPrivate();

        std::cout << "DrawingArea constructor completed" << std::endl;
        qDebug() << "DrawingArea constructor completed";
    } catch (const std::exception& e) {
        std::cerr << "Exception in DrawingArea constructor: " << e.what() << std::endl;
        qCritical() << "Exception in DrawingArea constructor:" << e.what();
    } catch (...) {
        std::cerr << "Unknown exception in DrawingArea constructor" << std::endl;
        qCritical() << "Unknown exception in DrawingArea constructor";
    }
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

    // Draw the gears
    drawGears(painter);
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

bool DrawingArea::exportToGcode(const QString &filename, const GcodeGenerator::Config& config) const
{
    return d_ptr->gcodeGenerator->generateGcode(spirographPaths, config, filename);
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

void DrawingArea::drawGears(QPainter &painter)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Draw outer gear
    drawGear(painter, QPointF(0, 0), outerRadius, Qt::black);

    // Calculate inner gear position
    double x = (outerRadius - innerRadius) * qCos(currentAngle);
    double y = (outerRadius - innerRadius) * qSin(currentAngle);
    QPointF innerGearCenter(x, y);

    // Draw inner gear
    painter.save();
    painter.translate(innerGearCenter);
    painter.rotate(currentAngle * outerRadius / innerRadius * 180 / M_PI);
    drawGear(painter, QPointF(0, 0), innerRadius, Qt::black);

    // Draw pen holes
    drawPenHoles(painter);

    painter.restore();

    // Draw arm connecting the gears
    //painter.setPen(QPen(Qt::black, 2 / zoomFactor));
    //painter.drawLine(QPointF(0, 0), innerGearCenter);

    // Draw current pen position
    QPointF penPosition = calculatePenPosition();
    painter.setBrush(Qt::red);
    painter.drawEllipse(penPosition, 3 / zoomFactor, 3 / zoomFactor);

    painter.restore();
}

void DrawingArea::drawGear(QPainter &painter, const QPointF &center, double radius, const QColor &color)
{
    painter.save();
    painter.translate(center);

    QPen gearPen(color);
    gearPen.setWidthF(2 / zoomFactor);
    painter.setPen(gearPen);
    painter.setBrush(Qt::NoBrush);

    // Draw the main circle of the gear
    painter.drawEllipse(QPointF(0, 0), radius, radius);

    // Draw center circle
    double centerRadius = radius * 0.1;
    painter.drawEllipse(QPointF(0, 0), centerRadius, centerRadius);

    painter.restore();
}

void DrawingArea::drawPenHoles(QPainter &painter)
{
    int holeCount = 8;  // Number of pen holes
    double holeRadius = innerRadius * 0.05;  // Size of the holes

    QPen holePen(Qt::black);
    holePen.setWidthF(1 / zoomFactor);
    painter.setPen(holePen);
    painter.setBrush(Qt::NoBrush);

    // Highlight the current pen hole
    //QPointF currentHole(penOffset, 0);
    //painter.setBrush(Qt::red);
    //painter.drawEllipse(currentHole, holeRadius, holeRadius);
}

QPointF DrawingArea::calculatePenPosition()
{
    double t = currentAngle;
    double x = (outerRadius - innerRadius) * qCos(t) + 
               penOffset * qCos(((outerRadius - innerRadius) * t / innerRadius) + rotationOffset);
    double y = (outerRadius - innerRadius) * qSin(t) - 
               penOffset * qSin(((outerRadius - innerRadius) * t / innerRadius) + rotationOffset);
    return QPointF(x, y);
}

void DrawingArea::startAnimation()
{
    if (!isAnimating) {
        isAnimating = true;
        currentAngle = 0;
        animationTimer->start(50);  // 20 FPS
    }
}

void DrawingArea::stopAnimation()
{
    if (isAnimating) {
        isAnimating = false;
        animationTimer->stop();
    }
}

void DrawingArea::updateAnimation()
{
    currentAngle += 0.05;  // Adjust this value to change the animation speed
    if (currentAngle >= 2 * M_PI * rotations) {
        stopAnimation();
    }
    update();
}