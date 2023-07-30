#include "controller/controller.hpp"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QIcon>
#include <QStandardPaths>
#include <QString>
#include <QTranslator>
#include <QtLogging>
#include <memory>

static const QString ACC_MAJOR_VERSION = QString("0");
static const QString ACC_MINOR_VERSION = QString("1");
static const QString ACC_PATCH_VERSION = QString("0");
static const QString VERSION_STRING = QString("%1.%2.%3").arg(ACC_MAJOR_VERSION, ACC_MINOR_VERSION, ACC_PATCH_VERSION);

inline int GetVersionNumber(const QString& str)
{
    QStringList online_version = str.split('.');
    if (online_version.size() != 3)
        return 0;

    int major = online_version[0].toInt();
    int minor = online_version[1].toInt();
    int patch = online_version[2].toInt();
    return major * 10000 + minor * 100 + patch;
}

// set logging system
static const QString LOG_FILENAME = QString("accountancy_log.txt");
std::unique_ptr<QFile> _log_file;
void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

int main(int argc, char* argv[])
{
    Q_INIT_RESOURCE(Resources);

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("KindVador");
    QCoreApplication::setApplicationName("Accountancy");
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QCoreApplication::setApplicationVersion(VERSION_STRING);

    // init LOG
    QDir log_dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!log_dir.exists()) {
        log_dir.cdUp();
        log_dir.mkdir(QCoreApplication::applicationName());
        log_dir.cd(QCoreApplication::applicationName());
    }
    _log_file = std::make_unique<QFile>(log_dir.absoluteFilePath(LOG_FILENAME));
    if (!_log_file->open(QIODevice::WriteOnly)) {
        qWarning() << "Unable to open file: " << _log_file->fileName();
    }
    qInstallMessageHandler(messageHandler);

    // set ICON
    QIcon app_icon(":/icns/resources/icons/accountancy.svg");
    QApplication::setWindowIcon(app_icon);

    // Parse command line arguments
    QString filePath;
    QStringList arguments = QCoreApplication::arguments();
    if (arguments.count() > 1) {
        qWarning() << arguments;
        filePath = arguments.at(1);
    }

    // Controller
    Controller* controller = Controller::instance();
    if (!filePath.isEmpty())
        controller->loadFile(filePath);
    else
        controller->loadFromSettings();
    controller->showMainWindow();

    // launching application
    return QApplication::exec();
}

void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    if (_log_file == nullptr)
        return;

    QTextStream out_stream(_log_file.get());
    out_stream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");
    switch (type) {
        case QtInfoMsg:
            out_stream << "INF ";
            break;
        case QtDebugMsg:
            out_stream << "DBG ";
            break;
        case QtWarningMsg:
            out_stream << "WRN ";
            break;
        case QtCriticalMsg:
            out_stream << "CRT ";
            break;
        case QtFatalMsg:
            out_stream << "FTL ";
            break;
    }

    out_stream << context.category << ": " << msg << "\n";
    out_stream.flush();
}
