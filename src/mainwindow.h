#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <map>

#include <QMainWindow>
#include <QString>

#include "dbtablewidget.h"
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
    void showAccountsTable();
    void showTransactionsTable();
    void showBanksTable();
    void showBalanceNotesTable();
    void showCategoriesTable();
    void showSubCategoriesTable();
    void showRetailersTable();

private:
    Ui::MainWindow *ui;
    DatabaseModel *dbModel;
    std::map<QString, DbTableWidget*> dbTableViews;

    // Private methods
    void createMenus();
    void showDbTable(const QString tableName);
};
#endif // MAINWINDOW_H
