#ifndef GCODEGENERATOR_H
#define GCODEGENERATOR_H

#include <QString>
#include <QVector>
#include <QPainterPath>
#include <QRectF>

class GcodeGenerator
{
public:
    enum class Origin {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        Center
    };

    struct Config {
        double drawingAreaWidth;
        double drawingAreaHeight;
        double maxSpeed;
        double maxAcceleration;
        double penUpPosition;
        double penDownPosition;
        double travelSpeed;
        double drawingSpeed;
        Origin origin;
        QString startGcode;
        QString endGcode;
    };

    GcodeGenerator();
    bool generateGcode(const QVector<QPainterPath>& paths, const Config& config, const QString& filename);

private:
    QString convertPathToGcode(const QPainterPath& path, const Config& config, const QRectF& boundingBox, double scale, double offsetX, double offsetY);
    QString moveToPoint(const QPointF& point, bool penDown, const Config& config);
    QString setPenPosition(bool down, const Config& config);
    QPointF applyOriginTransform(const QPointF& point, const Config& config, const QRectF& boundingBox, double scale);

    bool m_currentPenState;  // true if pen is down, false if pen is up
};

#endif // GCODEGENERATOR_H