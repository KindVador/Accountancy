#ifndef CURRENCYMODEL_H
#define CURRENCYMODEL_H

#include <QAbstractListModel>
#include <QDebug>
#include <QUuid>

#include "currency.hpp"

class CurrencyModel : public QAbstractListModel
{
    Q_OBJECT

public:
    CurrencyModel() = default;
    ~CurrencyModel() override = default;

    // QAbstractListModel interface
    [[nodiscard]] int rowCount(const QModelIndex& parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;

    void addCurrency(Currency* currency);
    Currency* addCurrency(const QString& name, const QString& symbol);
    Currency* addCurrency(QString&& name, QString&& symbol);
    void removeCurrency(Currency* currency);
    void removeCurrency(QUuid uid);
    [[nodiscard]] Currency* getCurrency(const QString& name) const;
    [[nodiscard]] Currency* getCurrency(QUuid uid) const;

    void reset();

private:
    QList<Currency*> _currencies;
};

#endif// CURRENCYMODEL_H
