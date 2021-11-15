#include "currencymodel.hpp"

const auto CURRENCIES_SQL = QLatin1String(R"(CREATE TABLE currencies(id INTEGER PRIMARY KEY,  name VARCHAR, symbol VARCHAR))");
const auto INSERT_CURRENCY_SQL = QLatin1String(R"(INSERT INTO currencies(name, symbol) values(:name, :symbol))");
const auto GET_CURRENCY_ID_BY_NAME = QLatin1String(R"(SELECT id FROM currencies WHERE name = :name)");

CurrencyModel::CurrencyModel(const QString tableName)
{
    this->setTable(tableName);
}

QSqlError CurrencyModel::addCurrency(const Currency &currency)
{
    return addCurrency(currency.getName(), currency.getSymbol());
}

QSqlError CurrencyModel::addCurrency(const QString &name, const QString &symbol)
{
    QSqlQuery query;
    query.prepare(INSERT_CURRENCY_SQL);
    query.bindValue(":name", name);
    query.bindValue(":symbol", symbol);
    if(query.exec()) {
        return QSqlError();
    } else {
        QSqlError err = query.lastError();
        qDebug() << "addCurrency error:" << err << Qt::endl;
        return err;
    }
}

int CurrencyModel::getCurrencyId(const QString &name)
{
    QSqlQuery query;
    query.prepare(GET_CURRENCY_ID_BY_NAME);
    query.bindValue(":name", name);
    if(query.exec()) {
        int fieldNo = query.record().indexOf("id");
        if (query.next()) {
            return query.value(fieldNo).toInt();
        } else {
            return -1;
        }
    } else {
        qDebug() << "ERROR: No row found in currencies table for name: " << name << Qt::endl;
        return -1;
    }
}
