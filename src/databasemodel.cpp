#include "databasemodel.h"
#include <iostream>
#include <QtSql>
#include <QDebug>

using namespace std;

DatabaseModel::DatabaseModel(QObject *parent) : QObject(parent) {
    initModel();
}

DatabaseModel::DatabaseModel(QString dbStr, QObject *parent) : QObject(parent), dbString(dbStr) {
    initModel();
}

DatabaseModel::~DatabaseModel() {
    // TODO iterate over models to delete all
}

void DatabaseModel::initModel() {
    connectToDatabase();

    // check if we are connected to the database
    if (!db.isOpen()) {
        // TODO raise an Error Message in a Dialog window.
        return;
    }

    // iterate on tables to create a model for each table
    QStringList tables = db.tables();
    for (int i = 0; i < tables.size(); ++i) {
        models[tables.at(i)] = new QSqlTableModel(this, db);
        models[tables.at(i)]->setTable(tables.at(i));
        models[tables.at(i)]->setEditStrategy(QSqlTableModel::OnFieldChange);
        models[tables.at(i)]->select();
    }
}

void DatabaseModel::setDatabaseString(QString dbStr) {
    dbString = dbStr;
}

QString DatabaseModel::getDatabaseString() {
    return dbString;
}

bool DatabaseModel::connectToDatabase() {
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

bool DatabaseModel::isConnected() {
    return db.isOpen();
}

void DatabaseModel::disconnectFromDatabase() {
    db.close();
}

QSqlTableModel* DatabaseModel::getModelForTable(QString tableName) {
    return models[tableName];
}
