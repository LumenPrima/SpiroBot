#include "gcodeexportdialog.h"
#include "gcodegenerator.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <QStandardPaths>

GcodeExportDialog::GcodeExportDialog(QWidget *parent)
    : QDialog(parent)
{
    qDebug() << "GcodeExportDialog constructor started";
    setWindowTitle(tr("Gcode Export Settings"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    // Drawing Area Width
    drawingAreaWidthSpinBox = new QDoubleSpinBox(this);
    drawingAreaWidthSpinBox->setRange(1, 1000);
    drawingAreaWidthSpinBox->setValue(200);
    drawingAreaWidthSpinBox->setSuffix(" mm");
    formLayout->addRow(tr("Drawing Area Width:"), drawingAreaWidthSpinBox);

    // Drawing Area Height
    drawingAreaHeightSpinBox = new QDoubleSpinBox(this);
    drawingAreaHeightSpinBox->setRange(1, 1000);
    drawingAreaHeightSpinBox->setValue(200);
    drawingAreaHeightSpinBox->setSuffix(" mm");
    formLayout->addRow(tr("Drawing Area Height:"), drawingAreaHeightSpinBox);

    // Max Speed
    maxSpeedSpinBox = new QDoubleSpinBox(this);
    maxSpeedSpinBox->setRange(1, 10000);
    maxSpeedSpinBox->setValue(3000);
    maxSpeedSpinBox->setSuffix(" mm/min");
    formLayout->addRow(tr("Max Speed:"), maxSpeedSpinBox);

    // Max Acceleration
    maxAccelerationSpinBox = new QDoubleSpinBox(this);
    maxAccelerationSpinBox->setRange(1, 10000);
    maxAccelerationSpinBox->setValue(500);
    maxAccelerationSpinBox->setSuffix(" mm/s²");
    formLayout->addRow(tr("Max Acceleration:"), maxAccelerationSpinBox);

    // Pen Up Position
    penUpPositionSpinBox = new QDoubleSpinBox(this);
    penUpPositionSpinBox->setRange(-50, 50);
    penUpPositionSpinBox->setValue(5);
    penUpPositionSpinBox->setSuffix(" mm");
    formLayout->addRow(tr("Pen Up Position:"), penUpPositionSpinBox);

    // Pen Down Position
    penDownPositionSpinBox = new QDoubleSpinBox(this);
    penDownPositionSpinBox->setRange(-50, 50);
    penDownPositionSpinBox->setValue(-1);
    penDownPositionSpinBox->setSuffix(" mm");
    formLayout->addRow(tr("Pen Down Position:"), penDownPositionSpinBox);

    // Travel Speed
    travelSpeedSpinBox = new QDoubleSpinBox(this);
    travelSpeedSpinBox->setRange(1, 10000);
    travelSpeedSpinBox->setValue(3000);
    travelSpeedSpinBox->setSuffix(" mm/min");
    formLayout->addRow(tr("Travel Speed:"), travelSpeedSpinBox);

    // Drawing Speed
    drawingSpeedSpinBox = new QDoubleSpinBox(this);
    drawingSpeedSpinBox->setRange(1, 10000);
    drawingSpeedSpinBox->setValue(1500);
    drawingSpeedSpinBox->setSuffix(" mm/min");
    formLayout->addRow(tr("Drawing Speed:"), drawingSpeedSpinBox);

    // Origin selection
    originComboBox = new QComboBox(this);
    originComboBox->addItem(tr("Bottom Left"), static_cast<int>(GcodeGenerator::Origin::BottomLeft));
    originComboBox->addItem(tr("Top Left"), static_cast<int>(GcodeGenerator::Origin::TopLeft));
    originComboBox->addItem(tr("Top Right"), static_cast<int>(GcodeGenerator::Origin::TopRight));
    originComboBox->addItem(tr("Bottom Right"), static_cast<int>(GcodeGenerator::Origin::BottomRight));
    originComboBox->addItem(tr("Center"), static_cast<int>(GcodeGenerator::Origin::Center));
    formLayout->addRow(tr("Origin:"), originComboBox);

    mainLayout->addLayout(formLayout);

    // Start Gcode
    QLabel *startGcodeLabel = new QLabel(tr("Start Gcode:"), this);
    startGcodeEdit = new QPlainTextEdit(this);
    mainLayout->addWidget(startGcodeLabel);
    mainLayout->addWidget(startGcodeEdit);

    // End Gcode
    QLabel *endGcodeLabel = new QLabel(tr("End Gcode:"), this);
    endGcodeEdit = new QPlainTextEdit(this);
    mainLayout->addWidget(endGcodeLabel);
    mainLayout->addWidget(endGcodeEdit);

    // Add some vertical spacing
    mainLayout->addSpacing(20);

    // Create and add the button box
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);

    // Load configuration after all UI elements are initialized
    qDebug() << "About to load configuration file";
    loadConfigurationFromFile();
    qDebug() << "GcodeExportDialog constructor completed";
}

void GcodeExportDialog::loadConfigurationFromFile()
{
    qDebug() << "Attempting to load configuration file...";

    // Try to find the config file in multiple locations
    QStringList searchPaths = {
        QCoreApplication::applicationDirPath() + "/../config.json",
        QCoreApplication::applicationDirPath() + "/config.json",
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/config.json"
    };

    QString configPath;
    for (const auto &path : searchPaths) {
        qDebug() << "Checking for config file at:" << path;
        if (QFile::exists(path)) {
            configPath = path;
            break;
        }
    }

    if (configPath.isEmpty()) {
        qWarning() << "Configuration file not found in any of the search paths";
        return;
    }

    qDebug() << "Found config file at:" << configPath;

    QFile configFile(configPath);
    if (!configFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open configuration file:" << configFile.errorString();
        return;
    }

    qDebug() << "Config file opened successfully";

    QByteArray saveData = configFile.readAll();
    qDebug() << "Read" << saveData.size() << "bytes from config file";

    QJsonParseError parseError;
    QJsonDocument loadDoc = QJsonDocument::fromJson(saveData, &parseError);
    
    if (loadDoc.isNull()) {
        qWarning() << "Failed to parse JSON from config file:" << parseError.errorString();
        return;
    }

    qDebug() << "JSON parsed successfully";

    QJsonObject json = loadDoc.object();

    qDebug() << "JSON object created, keys:" << json.keys().join(", ");

    // Load default values with error checking
    if (json.contains("defaultDrawingAreaWidth")) {
        qDebug() << "Setting defaultDrawingAreaWidth";
        drawingAreaWidthSpinBox->setValue(json["defaultDrawingAreaWidth"].toDouble(200));
    }
    if (json.contains("defaultDrawingAreaHeight")) {
        qDebug() << "Setting defaultDrawingAreaHeight";
        drawingAreaHeightSpinBox->setValue(json["defaultDrawingAreaHeight"].toDouble(200));
    }
    if (json.contains("defaultMaxSpeed")) {
        qDebug() << "Setting defaultMaxSpeed";
        maxSpeedSpinBox->setValue(json["defaultMaxSpeed"].toDouble(3000));
    }
    if (json.contains("defaultMaxAcceleration")) {
        qDebug() << "Setting defaultMaxAcceleration";
        maxAccelerationSpinBox->setValue(json["defaultMaxAcceleration"].toDouble(500));
    }
    if (json.contains("defaultPenUpPosition")) {
        qDebug() << "Setting defaultPenUpPosition";
        penUpPositionSpinBox->setValue(json["defaultPenUpPosition"].toDouble(5));
    }
    if (json.contains("defaultPenDownPosition")) {
        qDebug() << "Setting defaultPenDownPosition";
        penDownPositionSpinBox->setValue(json["defaultPenDownPosition"].toDouble(-1));
    }
    if (json.contains("defaultTravelSpeed")) {
        qDebug() << "Setting defaultTravelSpeed";
        travelSpeedSpinBox->setValue(json["defaultTravelSpeed"].toDouble(3000));
    }
    if (json.contains("defaultDrawingSpeed")) {
        qDebug() << "Setting defaultDrawingSpeed";
        drawingSpeedSpinBox->setValue(json["defaultDrawingSpeed"].toDouble(1500));
    }

    // Load default Gcodes
    if (json.contains("defaultStartGcode")) {
        qDebug() << "Setting defaultStartGcode";
        QJsonArray startGcodeArray = json["defaultStartGcode"].toArray();
        QString startGcode;
        for (const auto &line : startGcodeArray) {
            startGcode += line.toString() + "\n";
        }
        startGcodeEdit->setPlainText(startGcode);
    }

    if (json.contains("defaultEndGcode")) {
        qDebug() << "Setting defaultEndGcode";
        QJsonArray endGcodeArray = json["defaultEndGcode"].toArray();
        QString endGcode;
        for (const auto &line : endGcodeArray) {
            endGcode += line.toString() + "\n";
        }
        endGcodeEdit->setPlainText(endGcode);
    }

    qDebug() << "Configuration loaded successfully from:" << configPath;
    qDebug() << "Loaded values:";
    qDebug() << "  Drawing Area Width:" << drawingAreaWidthSpinBox->value();
    qDebug() << "  Drawing Area Height:" << drawingAreaHeightSpinBox->value();
    qDebug() << "  Max Speed:" << maxSpeedSpinBox->value();
    qDebug() << "  Max Acceleration:" << maxAccelerationSpinBox->value();
    qDebug() << "  Pen Up Position:" << penUpPositionSpinBox->value();
    qDebug() << "  Pen Down Position:" << penDownPositionSpinBox->value();
    qDebug() << "  Travel Speed:" << travelSpeedSpinBox->value();
    qDebug() << "  Drawing Speed:" << drawingSpeedSpinBox->value();
    qDebug() << "  Start Gcode:" << startGcodeEdit->toPlainText();
    qDebug() << "  End Gcode:" << endGcodeEdit->toPlainText();
}

GcodeGenerator::Config GcodeExportDialog::getConfig() const
{
    GcodeGenerator::Config config;
    config.drawingAreaWidth = drawingAreaWidthSpinBox->value();
    config.drawingAreaHeight = drawingAreaHeightSpinBox->value();
    config.maxSpeed = maxSpeedSpinBox->value();
    config.maxAcceleration = maxAccelerationSpinBox->value();
    config.penUpPosition = penUpPositionSpinBox->value();
    config.penDownPosition = penDownPositionSpinBox->value();
    config.travelSpeed = travelSpeedSpinBox->value();
    config.drawingSpeed = drawingSpeedSpinBox->value();
    config.origin = static_cast<GcodeGenerator::Origin>(originComboBox->currentData().toInt());
    config.startGcode = startGcodeEdit->toPlainText();
    config.endGcode = endGcodeEdit->toPlainText();
    return config;
}