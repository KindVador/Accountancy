#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QString>
#include <QDebug>
#include "accounttablewidget.h"
#include <QMdiSubWindow>
#include <QLabel>
#include <QtSql>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Init Ui
    ui->actionDisconnect->setEnabled(false);

    // Create Model instance
    dbModel = new DatabaseModel("/Users/florian/CppProjects/Accountancy/db/data.db");
//    QString dbStr = "/Users/florian/CppProjects/Accountancy/db/data.db";
//    dbModel->setDatabaseString(dbStr);

    // Connect Actions to Slots
    connect(ui->actionCredits, SIGNAL(triggered()), this, SLOT(showCredits()));
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(connectDatabase()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(disconnectDatabase()));
    connect(ui->actionOwner, SIGNAL(triggered()), this, SLOT(showOwnersTable()));

    // Add Menus
    createMenus();

    // Update of UI after database connection
    if (dbModel->isConnected()) {
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
    }

}

void MainWindow::createMenus()
{
    // create one Menu for each Account Owner recorded in the database
    // TODO read owners list from the database

}

MainWindow::~MainWindow()
{
    delete ui;
    delete dbModel;
}

void MainWindow::showCredits()
{
    QString text(tr("Icônes faites par Pixel perfect de www.flaticon.com"));  // In english, Icons made by Pixel perfect from www.flaticon.com
    QMessageBox::information(this, "Credits", text);
}

void MainWindow::connectDatabase()
{
    // open DB connection
    dbModel->disconnect();
    // Update UI
    ui->actionDisconnect->setEnabled(true);
    ui->actionConnect->setEnabled(false);
}

void MainWindow::disconnectDatabase()
{
    // close DB connection
    dbModel->disconnect();
    // Update UI
    ui->actionDisconnect->setEnabled(false);
    ui->actionConnect->setEnabled(true);
}

void MainWindow::showOwnersTable()
{
    qDebug() << "show Table: Owners";
    AccountTableWidget* ownerTableWidget = new AccountTableWidget();
    ownerTableWidget->setTitle("Account's owner table:");
    ownerTableWidget->setTableModel(dbModel->getOwnerModel());
    dbModel->getOwnerModel()->select();
    QMdiSubWindow *ownerWindow = ui->mdiArea->addSubWindow(ownerTableWidget);
    ownerWindow->setAttribute(Qt::WA_DeleteOnClose);
    ownerWindow->setFocus();
    ownerWindow->show();
}
