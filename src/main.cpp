#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>
#include <QString>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

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
