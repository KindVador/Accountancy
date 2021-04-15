#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>
#include <QString>
#include <QSettings>
#include <QDir>
#include <iostream>
#include <QDebug>

static QString VERSION_STRING = QString("%1.%2.%3").arg(ACC_MAJOR_VERSION).arg(ACC_MINOR_VERSION).arg(ACC_PATCH_VERSION);

inline int GetVersionNumber(QString str)
{
  QStringList online_version = str.split('.');
  if( online_version.size() != 3 )
  {
    return 0;
  }
  int major = online_version[0].toInt();
  int minor = online_version[1].toInt();
  int patch = online_version[2].toInt();
  return major * 10000 + minor * 100 + patch;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("KindVador");
    QCoreApplication::setApplicationName("Accountancy");
    QSettings::setDefaultFormat(QSettings::IniFormat);

    QSettings settings;
    if( !settings.isWritable())
    {
        qDebug() << "ERROR: the file [" << settings.fileName() << "] is not writtable.";
    }

    // set ICON
    QIcon app_icon("://resources/plotjuggler.svg");
    QApplication::setWindowIcon(app_icon);

    // Traduction de l'application
    QString locale = QLocale::system().name();   //.section('_', 0, 0);
    QTranslator translator;
    translator.load("Accountancy_" + locale);
//    translator.load("Accountancy_en_US");
    app.installTranslator(&translator);

    // Création de la fenêtre principale
    MainWindow w;
    w.show();

    // Lancement de l'application
    return app.exec();
}
