#include "dbmanager.hpp"

#include <QFile>

const auto CURRENCIES_SQL = QLatin1String(R"(CREATE TABLE currencies(id INTEGER PRIMARY KEY,  name VARCHAR, symbol VARCHAR))");

const auto OWNERS_SQL = QLatin1String(R"(CREATE TABLE owners(id INTEGER PRIMARY KEY, name VARCHAR, currency INTEGER, warningbalance REAL, comment VARCHAR, ishidden INTEGER,
FOREIGN KEY(currency) REFERENCES currencies(id)))");


DbManager::DbManager(const QString& path)
{
    // test if path exist on disk
    if (!QFile::exists(path)) {
        qDebug() << "create a new empty database" << Qt::endl;
        createDb(path);
    } else {
        qDebug() << "open database file: " << path << Qt::endl;
        openDb(path);
    }

}

QSqlError DbManager::createDb(const QString &path)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

    if (!db.open())
        return db.lastError();

    // NOT EVERYONE WILL HAVE FOREIGN KEYS ENABLED.
    // SO THE SQLITE3 DEVS GAVE US THIS LITTLE GEM TO ENSURE FOREIGN KEY ACCESS.
    QSqlQuery query(db);
    query.exec("PRAGMA foreign_keys = ON;");

    QStringList tables = db.tables();
    if (tables.contains("currencies", Qt::CaseInsensitive) && tables.contains("owners", Qt::CaseInsensitive))
        return QSqlError();

    QSqlQuery q;
    if (!q.exec(CURRENCIES_SQL))
        return q.lastError();
    if (!q.exec(OWNERS_SQL))
        return q.lastError();

    return QSqlError();
}

QSqlError DbManager::openDb(const QString &path)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

    if (!db.open()) {
        return db.lastError();
    } else {
        return QSqlError();
    }

}

bool DbManager::isConnected()
{
    return db.isOpen();
}


QSqlDatabase DbManager::getDb() const
{
    return db;
}

void DbManager::setDb(const QSqlDatabase &value)
{
    db = value;
}
