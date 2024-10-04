#include "gcodegenerator.h"
#include <QTextStream>
#include <QFile>
#include <QFileInfo>  // Add this line
#include <QRectF>
#include <QtMath>

GcodeGenerator::GcodeGenerator() : m_currentPenState(false) {}

bool GcodeGenerator::generateGcode(const QVector<QPainterPath>& paths, const Config& config, const QString& filename)
{
    try {
        // Calculate bounding box of all paths
        QRectF boundingBox;
        for (const auto& path : paths) {
            boundingBox = boundingBox.united(path.boundingRect());
        }

        // Calculate scaling factors
        double scaleX = config.drawingAreaWidth / boundingBox.width();
        double scaleY = config.drawingAreaHeight / boundingBox.height();
        double scale = qMin(scaleX, scaleY);

        // Calculate offsets to shift the drawing into positive space
        double offsetX = -boundingBox.left() * scale;
        double offsetY = -boundingBox.top() * scale;

        // Generate Gcode for each pen
        for (int penNumber = 0; penNumber < paths.size(); ++penNumber) {
            QString penGcode;
            QTextStream stream(&penGcode);

            // Custom start Gcode
            stream << config.startGcode << "\n";
            
            // Set default feed rate
            stream << QString("F%1 ; Set default feed rate\n").arg(config.travelSpeed);
            
            // Initialize pen to up position
            stream << setPenPosition(false, config);
            m_currentPenState = false;

            // Convert path to Gcode
            if (penNumber < paths.size()) {
                stream << convertPathToGcode(paths[penNumber], config, boundingBox, scale, offsetX, offsetY);
            } else {
                qWarning() << "Attempted to access out-of-bounds path";
                return false;
            }

            // Custom end Gcode
            stream << config.endGcode << "\n";

            // Generate filename for this pen
            QFileInfo fileInfo(filename);
            QString penFilename = QString("%1_pen%2%3")
                                    .arg(fileInfo.completeBaseName())
                                    .arg(penNumber + 1)
                                    .arg(fileInfo.suffix().isEmpty() ? "" : "." + fileInfo.suffix());

            // Write to file
            QFile file(penFilename);
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                return false;
            }

            QTextStream fileStream(&file);
            fileStream << penGcode;
            file.close();
        }
        
        return true;
    } catch (const std::exception& e) {
        qCritical() << "Exception in generateGcode:" << e.what();
        return false;
    } catch (...) {
        qCritical() << "Unknown exception in generateGcode";
        return false;
    }
}

QString GcodeGenerator::convertPathToGcode(const QPainterPath& path, const Config& config, const QRectF& boundingBox, double scale, double offsetX, double offsetY)
{
    QString gcode;
    QTextStream stream(&gcode);

    for (int i = 0; i < path.elementCount(); ++i) {
        QPainterPath::Element el = path.elementAt(i);
        QPointF scaledPoint(el.x * scale + offsetX, el.y * scale + offsetY);
        QPointF transformedPoint = applyOriginTransform(scaledPoint, config, boundingBox, scale);
        
        if (el.isMoveTo()) {
            stream << moveToPoint(transformedPoint, false, config);
        } else if (el.isLineTo()) {
            stream << moveToPoint(transformedPoint, true, config);
        }
        // Handle curve elements if needed
    }

    return gcode;
}
QString GcodeGenerator::moveToPoint(const QPointF& point, bool penDown, const Config& config)
{
    QString gcode;
    QTextStream stream(&gcode);

    // Only change pen position if it's different from the current state
    if (penDown != m_currentPenState) {
        stream << setPenPosition(penDown, config);
        m_currentPenState = penDown;
    }

    stream << QString("G1 X%1 Y%2 F%3\n")
        .arg(point.x(), 0, 'f', 3)
        .arg(point.y(), 0, 'f', 3)
        .arg(penDown ? config.drawingSpeed : config.travelSpeed, 0, 'f', 0);

    return gcode;
}

QString GcodeGenerator::setPenPosition(bool down, const Config& config)
{
    return QString("G1 Z%1 F%2\n")
        .arg(down ? config.penDownPosition : config.penUpPosition, 0, 'f', 3)
        .arg(config.maxSpeed, 0, 'f', 0);
}

QPointF GcodeGenerator::applyOriginTransform(const QPointF& point, const Config& config, const QRectF& boundingBox, double scale)
{
    QPointF transformedPoint = point;
    double scaledWidth = boundingBox.width() * scale;
    double scaledHeight = boundingBox.height() * scale;

    switch (config.origin) {
        case Origin::TopLeft:
            transformedPoint.setY(config.drawingAreaHeight - transformedPoint.y());
            break;
        case Origin::TopRight:
            transformedPoint.setX(config.drawingAreaWidth - transformedPoint.x());
            transformedPoint.setY(config.drawingAreaHeight - transformedPoint.y());
            break;
        case Origin::BottomLeft:
            // No transformation needed
            break;
        case Origin::BottomRight:
            transformedPoint.setX(config.drawingAreaWidth - transformedPoint.x());
            break;
        case Origin::Center:
            transformedPoint.setX(transformedPoint.x() + (config.drawingAreaWidth - scaledWidth) / 2);
            transformedPoint.setY(transformedPoint.y() + (config.drawingAreaHeight - scaledHeight) / 2);
            break;
    }

    return transformedPoint;
}