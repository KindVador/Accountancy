#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dbtablewidget.h"
#include "maindockwidget.h"

#include <QMessageBox>
#include <QString>
#include <QDebug>
#include <QMdiSubWindow>
#include <QLabel>
#include <QtSql>
#include <QModelIndex>
#include <QAction>

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {

    ui->setupUi(this);

    // Init Ui
    ui->actionDisconnect->setEnabled(false);

    // Create Model instance
    dbModel = new DatabaseModel("/Users/florian/CppProjects/Accountancy/db/data.db");

    // Connect Actions to Slots
    connect(ui->actionCredits, SIGNAL(triggered()), this, SLOT(showCredits()));
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(connectDatabase()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(disconnectDatabase()));
    connect(ui->actionOwner, SIGNAL(triggered()), this, SLOT(showOwnersTable()));
    connect(ui->actionBanks, SIGNAL(triggered()), this, SLOT(showBanksTable()));
    connect(ui->actionAccounts, SIGNAL(triggered()), this, SLOT(showAccountsTable()));
    connect(ui->actionTransactions, SIGNAL(triggered()), this, SLOT(showTransactionsTable()));
    connect(ui->actionBalanceNotes, SIGNAL(triggered()), this, SLOT(showBalanceNotesTable()));
    connect(ui->actionCategories, SIGNAL(triggered()), this, SLOT(showCategoriesTable()));
    connect(ui->actionSubcategories, SIGNAL(triggered()), this, SLOT(showSubCategoriesTable()));
    connect(ui->actionRetailers, SIGNAL(triggered()), this, SLOT(showRetailersTable()));

    // Add Menus
    createMenus();

    //
    MainDockWidget* sidePanel = new MainDockWidget(this);
    addDockWidget(Qt::LeftDockWidgetArea, sidePanel);

    // Update of UI after database connection
    if (dbModel->isConnected()) {
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
    }

}

void MainWindow::createMenus() {
    // create one Menu for each Account Owner recorded in the database
    // TODO read owners list from the database
    int i = 0;
    QModelIndex idx;
    QVariant v;
    bool stopIterate = false;
    do {
        idx = dbModel->getModelForTable("account_owner")->index(i, 1);
        v = dbModel->getModelForTable("account_owner")->data(idx);
        if (v.isValid())
        {
            accountsActions[v.toString()] = ui->menuAccounts->addAction(v.toString(), OwnerDynamicSlot(this, v.toString()));
            accountsActions[v.toString()]->setCheckable(true);
            if (i == 0) {
                // select first owner by default
                accountsActions[v.toString()]->setChecked(true);
                setCurrentOwner(v.toString());
            }

            qDebug() << i << " " << v.toString();
            ++i;
        } else {
            stopIterate = true;
        }
    } while(!stopIterate);

}

MainWindow::~MainWindow() {
    delete ui;
    delete dbModel;
    // TODO need to iterate on dbTableViews to delete all items ?
}


QString MainWindow::getCurrentOwner() {
    return currentOwner;
}

void MainWindow::setCurrentOwner(QString owner) {
    currentOwner = owner;
    // TODO update UI with the new current Owner
    qDebug() << "MainWindow::setCurrentOwner" << owner;
}

void MainWindow::showCredits() {
    QString text(tr("Icônes faites par Pixel perfect de www.flaticon.com"));  // In english, Icons made by Pixel perfect from www.flaticon.com
    QMessageBox::information(this, "Credits", text);
}

void MainWindow::connectDatabase() {
    // open DB connection
    dbModel->disconnect();
    // Update UI
    ui->actionDisconnect->setEnabled(true);
    ui->actionConnect->setEnabled(false);
}

void MainWindow::disconnectDatabase() {
    // close DB connection
    dbModel->disconnect();
    // Update UI
    ui->actionDisconnect->setEnabled(false);
    ui->actionConnect->setEnabled(true);
}

void MainWindow::showDbTable(const QString tableName) {
    qDebug() << "show Table: Owners";
    DbTableWidget* tableWidget = new DbTableWidget();
    tableWidget->setTitle("TABLE: " + tableName);
    tableWidget->setTableModel(dbModel->getModelForTable(tableName));
    QMdiSubWindow *ownerWindow = ui->mdiArea->addSubWindow(tableWidget);
    ownerWindow->setAttribute(Qt::WA_DeleteOnClose);
    ownerWindow->setFocus();
    ownerWindow->show();
    // add this table view in the map
    dbTableViews[tableName] = tableWidget;
}

void MainWindow::showOwnersTable() {
    showDbTable("account_owner");
}

void MainWindow::showAccountsTable() {
    showDbTable("bank_account");
}

void MainWindow::showTransactionsTable() {
    showDbTable("transaction_");
}

void MainWindow::showBanksTable() {
    showDbTable("bank");
}

void MainWindow::showBalanceNotesTable() {
    showDbTable("balance_note");
}

void MainWindow::showCategoriesTable() {
    showDbTable("sub_category");
}

void MainWindow::showSubCategoriesTable() {
    showDbTable("sub_category");
}

void MainWindow::showRetailersTable() {
    showDbTable("retailer");
}
