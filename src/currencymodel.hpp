#ifndef CURRENCYMODEL_H
#define CURRENCYMODEL_H

#include <QAbstractListModel>
#include <QDebug>

#include "core/currency.hpp"

class CurrencyModel : public QAbstractListModel
{
    Q_OBJECT

public:
    CurrencyModel() = default;
    ~CurrencyModel() override = default;

    // QAbstractListModel interface
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    int addCurrency(Currency *currency);
    Currency *addCurrency(QString &name, QString &symbol);
    void removeCurrency(Currency *currency);
    void removeCurrency(int id);
    Currency* getCurrency(const QString &name);

private:
    QList<Currency*> _currencies;

    [[nodiscard]] int getLastId() const;
};

#endif // CURRENCYMODEL_H
