#include "databasemodel.h"
#include <iostream>
#include <QtSql>

using namespace std;

DatabaseModel::DatabaseModel(QObject *parent) : QObject(parent)
{
    initModel();
}

DatabaseModel::~DatabaseModel()
{
    delete ownerModel;
}

void DatabaseModel::initModel()
{
    connectToDatabase();

    // check if we are connected to the database
    if (!db.isOpen()) {
        // TODO raise an Error Message in a Dialog window.
        return;
    }

    // Account Owner
    ownerModel = new QSqlTableModel(this, db);
    ownerModel->setTable("account_owner");
    ownerModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    ownerModel->select();
    ownerModel->setHeaderData(0, Qt::Horizontal, tr("PK"));
    ownerModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
}

void DatabaseModel::setDatabaseString(QString dbStr)
{
    dbString = dbStr;
}

QString DatabaseModel::getDatabaseString()
{
    return dbString;
}

bool DatabaseModel::connectToDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbString);
    if (db.open()) {
        // success!
        cout << "Connected to the database" << endl;
        return true;
    }
    else {
        // error while opening database connection
        QSqlError error = db.lastError();
        cout << error.text().toCFString() << endl;
        return false;
    }
}

bool DatabaseModel::isConnected()
{
    return db.isOpen();
}

void DatabaseModel::disconnectFromDatabase()
{
    db.close();
}

QSqlTableModel* DatabaseModel::getOwnerModel()
{
    return ownerModel;
}
