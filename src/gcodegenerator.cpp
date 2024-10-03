#include "gcodegenerator.h"
#include <QTextStream>
#include <QFile>

GcodeGenerator::GcodeGenerator() {}

bool GcodeGenerator::generateGcode(const QVector<QPainterPath>& paths, const Config& config, const QString& filename)
{
    QString gcode;
    QTextStream stream(&gcode);

    // Gcode preamble
    stream << "G21 ; Set units to millimeters\n";
    stream << "G90 ; Use absolute coordinates\n";
    stream << "G92 X0 Y0 Z0 ; Set current position as home\n";
    stream << "M5 ; Ensure spindle is off\n";
    stream << QString("F%1 ; Set default feed rate\n").arg(config.travelSpeed);
    stream << setPenPosition(false, config);  // Ensure pen is up

    for (const auto& path : paths) {
        stream << convertPathToGcode(path, config);
    }

    // Gcode postamble
    stream << setPenPosition(false, config);  // Ensure pen is up
    stream << "G0 X0 Y0 ; Return to home position\n";
    stream << "M5 ; Ensure spindle is off\n";
    stream << "M2 ; End program\n";

    // Write to file
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream fileStream(&file);
    fileStream << gcode;
    file.close();

    return true;
}

QString GcodeGenerator::convertPathToGcode(const QPainterPath& path, const Config& config)
{
    QString gcode;
    QTextStream stream(&gcode);

    for (int i = 0; i < path.elementCount(); ++i) {
        QPainterPath::Element el = path.elementAt(i);
        
        if (el.isMoveTo()) {
            stream << moveToPoint(QPointF(el.x, el.y), false, config);
        } else if (el.isLineTo()) {
            stream << moveToPoint(QPointF(el.x, el.y), true, config);
        }
        // Handle curve elements if needed
    }

    return gcode;
}

QString GcodeGenerator::moveToPoint(const QPointF& point, bool penDown, const Config& config)
{
    QString gcode;
    QTextStream stream(&gcode);

    stream << setPenPosition(penDown, config);

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