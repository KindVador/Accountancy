#include "databasemodel.h"
#include <iostream>
#include <QtSql>
#include <QDebug>

using namespace std;

DatabaseModel::DatabaseModel(QObject *parent) : QObject(parent)
{
    initModel();
}

DatabaseModel::DatabaseModel(QString dbStr, QObject *parent) : QObject(parent), dbString(dbStr)
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
    ownerModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    ownerModel->setHeaderData(0, Qt::Horizontal, tr("PK"));
    ownerModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
    ownerModel->select();
    qDebug() << ownerModel->record(0).count();
    qDebug() << ownerModel->record(0).field(0);
    qDebug() << ownerModel->record(0).field(1);
    qDebug() << db.isValid();
    qDebug() << db.tables().join(";");
    qDebug() << db.lastError();
    qDebug() << db.driverName();
    qDebug() << db.isDriverAvailable("QSQLITE");
    qDebug() << db.record("account_owner");

    //
    QSqlQuery query;
    query.prepare("SELECT * FROM account_owner");
    if(!query.exec())
      qWarning() << "ERROR: " << query.lastError().text();
    if(query.first())
      qDebug() << query.value(0).toString();
    else
      qDebug() << "No account owner found";
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
