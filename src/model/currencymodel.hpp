#ifndef CURRENCYMODEL_H
#define CURRENCYMODEL_H

#include <QAbstractListModel>
#include <QDebug>
#include <QUuid>

#include "abstractmodel.hpp"
#include "core/currency.hpp"

class CurrencyModel : public QAbstractListModel, public AbstractModel
{
    Q_OBJECT

public:
    explicit CurrencyModel(QString name);
    ~CurrencyModel() override = default;

    // AbstractModel
    [[nodiscard]] bool isDirty() const override;
    void reset() override;

    // QAbstractListModel interface
    [[nodiscard]] int rowCount(const QModelIndex& parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;

    void addCurrency(Currency* currency);
    Currency* addCurrency(const QString& name, const QString& symbol);
    void removeCurrency(const Currency* currency);
    void removeCurrency(QUuid uid);
    [[nodiscard]] Currency* getCurrency(const QString& name) const;
    [[nodiscard]] Currency* getCurrency(QUuid uid) const;

private:
    QList<Currency*> _currencies;
};

#endif// CURRENCYMODEL_H
