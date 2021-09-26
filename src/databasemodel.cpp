#include "databasemodel.hpp"
#include <iostream>
#include <QtSql>
#include <QDebug>

using namespace std;

DatabaseModel::DatabaseModel(QString path, QObject *parent) : QObject(parent) {
    dbm = new DbManager(path);
    initModel();
}

DatabaseModel::~DatabaseModel() {
    delete dbm;
}

void DatabaseModel::initModel() {
    connectToDatabase();

    // check if we are connected to the database
    if (!dbm->getDb().isOpen()) {
        // TODO raise an Error Message in a Dialog window.
        return;
    }

    // iterate on tables to create a model for each table
    QStringList tables = dbm->getDb().tables();
    for (int i = 0; i < tables.size(); ++i) {
        models[tables.at(i)] = new QSqlTableModel(this, dbm->getDb());
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
    if (dbm->getDb().open()) {
        // success!
        qDebug() << "Connected to the database" << Qt::endl;
        return true;
    }
    else {
        // error while opening database connection
        QSqlError error = dbm->getDb().lastError();
        qDebug() << error.text() << Qt::endl;
        return false;
    }
}

bool DatabaseModel::isConnected() {
    return dbm->getDb().isOpen();
}

void DatabaseModel::disconnectFromDatabase() {
    dbm->getDb().close();
}

DbManager *DatabaseModel::getDbm() const
{
    return dbm;
}

void DatabaseModel::setDbm(DbManager *value)
{
    dbm = value;
}

QSqlError DatabaseModel::addOwner(const QString &name, const QString &currencyName, const float &wngBalance, const QString &comment, const bool &hidden)
{
    int currencyId = this->getCurrencyModel()->getCurrencyId(currencyName);
    return this->getOwnerModel()->addOwner(name, currencyId, wngBalance, comment, hidden);
}

OwnerModel *DatabaseModel::getOwnerModel() const
{
    return ownerModel;
}

void DatabaseModel::setOwnerModel(OwnerModel *value)
{
    ownerModel = value;
}

CurrencyModel *DatabaseModel::getCurrencyModel() const
{
    return currencyModel;
}

void DatabaseModel::setCurrencyModel(CurrencyModel *value)
{
    currencyModel = value;
}

QSqlTableModel* DatabaseModel::getModelForTable(QString tableName) {
    return models[tableName];
}
