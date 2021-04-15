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
    QString getCurrentOwner();
    void setCurrentOwner(QString owner);

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

    void on_actionImport_triggered();

private:
    Ui::MainWindow *ui;
    DatabaseModel *dbModel;
    std::map<QString, DbTableWidget*> dbTableViews;
    std::map<QString, QAction*> accountsActions;
    QString currentOwner;

    // Private methods
    void createMenus();
    void showDbTable(const QString tableName);
};


/*
 * This class is a functor to connect actions which are create dynamicly at runtime
*/

class OwnerDynamicSlot
{
public:
    OwnerDynamicSlot(MainWindow* mw, QString owner) : mainWindow(mw), name(owner)
    {

    }

    void operator()()
    {
        mainWindow->setCurrentOwner(name);
    }

private:
    MainWindow* mainWindow;
    QString name;
};

#endif // MAINWINDOW_H
