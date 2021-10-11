#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QString>
#include <QtSql>
#include <QSqlDatabase>
#include <map>

#include "core/currency.hpp"
#include "core/owner.hpp"


class DbManager
{
public:
    DbManager(const QString& path);
    QSqlError createDb(const QString& path);
    QSqlError openDb(const QString& path);
    bool isConnected();

    QSqlDatabase getDb() const;
    void setDb(const QSqlDatabase &value);

private:
    QSqlDatabase db;
    std::map<QString, QSqlTableModel*> models;
};

#endif // DBMANAGER_H
