#include "controller.hpp"

#include <QApplication>
#include <QTranslator>
#include <QString>
#include <QSettings>
#include <QDir>

static QString ACC_MAJOR_VERSION = QString("1");
static QString ACC_MINOR_VERSION = QString("0");
static QString ACC_PATCH_VERSION = QString("0");
static QString VERSION_STRING = QString("%1.%2.%3").arg(ACC_MAJOR_VERSION, ACC_MINOR_VERSION, ACC_PATCH_VERSION);

inline int GetVersionNumber(QString &str)
{
  QStringList online_version = str.split('.');
  if( online_version.size() != 3 ) {
    return 0;
  }
  int major = online_version[0].toInt();
  int minor = online_version[1].toInt();
  int patch = online_version[2].toInt();
  return major * 10000 + minor * 100 + patch;
}

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(Resources);

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("KindVador");
    QCoreApplication::setApplicationName("Accountancy");
    QSettings::setDefaultFormat(QSettings::IniFormat);

    QSettings settings;
    if( !settings.isWritable())
        qDebug() << "ERROR: the file [" << settings.fileName() << "] is not writable.";

    // set ICON
    QIcon app_icon(":/imgs/accountancy.svg");
    QApplication::setWindowIcon(app_icon);

    // Controller
    Controller *controller = Controller::getInstance();
    controller->showMainWindow();

    // Lancement de l'application
    return app.exec();
}
