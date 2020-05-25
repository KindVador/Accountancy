#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "databasemodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showCredits();
    void connectDatabase();
    void disconnectDatabase();
    void showOwnersTable();

private:
    Ui::MainWindow *ui;
    DatabaseModel *dbModel;

    // Private methods
    void createMenus();
};
#endif // MAINWINDOW_H
