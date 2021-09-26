#ifndef CURRENCYMODEL_H
#define CURRENCYMODEL_H

#include <QSql>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

#include "currency.hpp"

class CurrencyModel : public QSqlTableModel
{
public:
    CurrencyModel(const QString tableName);
    QSqlError addCurrency(const Currency &currency);
    QSqlError addCurrency(const QString &name, const QString &symbol);
    int getCurrencyId(const QString &name);

};

#endif // CURRENCYMODEL_H
