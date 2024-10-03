#ifndef GCODEGENERATOR_H
#define GCODEGENERATOR_H

#include <QString>
#include <QVector>
#include <QPainterPath>

class GcodeGenerator
{
public:
    struct Config {
        double drawingAreaWidth;
        double drawingAreaHeight;
        double maxSpeed;
        double maxAcceleration;
        double penUpPosition;
        double penDownPosition;
        double travelSpeed;
        double drawingSpeed;
    };

    GcodeGenerator();
    bool generateGcode(const QVector<QPainterPath>& paths, const Config& config, const QString& filename);

private:
    QString convertPathToGcode(const QPainterPath& path, const Config& config);
    QString moveToPoint(const QPointF& point, bool penDown, const Config& config);
    QString setPenPosition(bool down, const Config& config);
};

#endif // GCODEGENERATOR_H