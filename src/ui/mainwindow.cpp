#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "../dbtablewidget.hpp"
#include "../owner.hpp"

#include <QMessageBox>
#include <QString>
#include <QDebug>
#include <QMdiSubWindow>
#include <QLabel>
#include <QtSql>
#include <QModelIndex>
#include <QAction>
#include <vector>
#include <QFileDialog>

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {

    ui->setupUi(this);

    // Init Ui
    ui->actionDisconnect->setEnabled(false);

    // Create Model instance
    QString dbPath = QString("/Users/florian/Library/Mobile Documents/com~apple~CloudDocs/Projects/Accountancy/db/data2.db");
    dbModel = new DatabaseModel(dbPath, this);

    // Connect Actions to Slots
    connect(ui->actionCredits, SIGNAL(triggered()), this, SLOT(showCredits()));
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(connectDatabase()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(disconnectDatabase()));

    // Init of UI after database connection
    if (dbModel->isConnected()) {
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);

        updateOwners();

    } else {
        qDebug() << "ERROR: no connection  to Database to initialize" << Qt::endl;
    }

}

void MainWindow::updateOwners() {
    qDebug() << "MainWindow::updateOwners()" << Qt::endl;

    // For each Owner, create one Menu and add button in the left panel
    std::vector<Owner> owners = dbModel->getOwnerModel()->getOwners();
    qDebug() << owners.size() << Qt::endl;
    for(auto &owner : owners) {
        qDebug() << owner.getId() << " " << owner.getName() << " " << owner.getCurrency().getName() << Qt::endl;
    }
    // TODO read owners list from the database
//    int i = 0;
//    QModelIndex idx;
//    QVariant v;
//    bool stopIterate = false;
//    do {
//        idx = dbModel->getModelForTable("account_owner")->index(i, 1);
//        v = dbModel->getModelForTable("account_owner")->data(idx);
//        if (v.isValid())
//        {
//            accountsActions[v.toString()] = ui->menuAccounts->addAction(v.toString(), OwnerDynamicSlot(this, v.toString()));
//            accountsActions[v.toString()]->setCheckable(true);
//            if (i == 0) {
//                // select first owner by default
//                accountsActions[v.toString()]->setChecked(true);
//                setCurrentOwner(v.toString());
//            }

//            qDebug() << i << " " << v.toString();
//            ++i;
//        } else {
//            stopIterate = true;
//        }
//    } while(!stopIterate);

}

MainWindow::~MainWindow()
{
    delete ui;
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
    if (!dbModel->isConnected())
        dbModel->getDbm()->getDb().open();
    // Update UI
    ui->actionDisconnect->setEnabled(true);
    ui->actionConnect->setEnabled(false);
}

void MainWindow::disconnectDatabase() {
    // close DB connection
    dbModel->getDbm()->getDb().close();
    // Update UI
    ui->actionDisconnect->setEnabled(false);
    ui->actionConnect->setEnabled(true);
}

//void MainWindow::showDbTable(const QString tableName) {
//    qDebug() << "show Table: Owners";
//    DbTableWidget* tableWidget = new DbTableWidget();
//    tableWidget->setTitle("TABLE: " + tableName);
//    tableWidget->setTableModel(dbModel->getModelForTable(tableName));
//    QMdiSubWindow *ownerWindow = ui->mdiArea->addSubWindow(tableWidget);
//    ownerWindow->setAttribute(Qt::WA_DeleteOnClose);
//    ownerWindow->setFocus();
//    ownerWindow->show();
//    // add this table view in the map
//    dbTableViews[tableName] = tableWidget;
//}

//void MainWindow::showOwnersTable() {
//    showDbTable("account_owner");
//}

//void MainWindow::showAccountsTable() {
//    showDbTable("bank_account");
//}

//void MainWindow::showTransactionsTable() {
//    showDbTable("transaction_");
//}

//void MainWindow::showBanksTable() {
//    showDbTable("bank");
//}

//void MainWindow::showBalanceNotesTable() {
//    showDbTable("balance_note");
//}

//void MainWindow::showCategoriesTable() {
//    showDbTable("sub_category");
//}

//void MainWindow::showSubCategoriesTable() {
//    showDbTable("sub_category");
//}

//void MainWindow::showRetailersTable() {
//    showDbTable("retailer");
//}

void MainWindow::on_actionImport_triggered()
{
    // TODO create a window to import data into DataBase
    qDebug() << "MainWindow::on_actionImport_triggered()" << Qt::endl;
    QFileDialog fileDlg(this);
    fileDlg.setFileMode(QFileDialog::ExistingFiles);
    fileDlg.setViewMode(QFileDialog::Detail);

    QStringList fileNames;
    if (fileDlg.exec()) {
        fileNames = fileDlg.selectedFiles();
    }

    // printing all selected file
    QStringList::const_iterator constIterator;
    for (constIterator = fileNames.constBegin(); constIterator != fileNames.constEnd(); ++constIterator) {
        qDebug() << *constIterator << Qt::endl;
        QFile dataFile(*constIterator);
        if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text))
                return;

            QTextStream in(&dataFile);
            while (!in.atEnd()) {
                QString line = in.readLine();
                qDebug() << line;
            }
    }
}
