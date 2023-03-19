#include "currencymodel.hpp"

#include <utility>

constexpr const int ObjectRole = Qt::UserRole + 1;

CurrencyModel::CurrencyModel(QString name) : AbstractModel(std::move(name))
{
}

int CurrencyModel::rowCount(const QModelIndex& parent) const
{
    return (int) _currencies.size();
}

QVariant CurrencyModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= _currencies.count() || index.row() < 0)
        return {};

    QVariant v;
    switch (role) {
        case Qt::DisplayRole:
            v.setValue(_currencies.at(index.row())->getDisplayedName());
            break;
        case Qt::UserRole:
            v.setValue(_currencies.at(index.row())->getUid());
            break;
        case ObjectRole:
            v.setValue(_currencies.at(index.row()));
            break;
        default:
            break;
    }

    return v;
}

void CurrencyModel::addCurrency(Currency* currency)
{
    if (currency == nullptr)
        return;

    beginResetModel();
    _currencies.append(currency);
    endResetModel();
}

void CurrencyModel::removeCurrency(const Currency* currency)
{
    beginResetModel();
    auto res = std::find_if(_currencies.cbegin(), _currencies.cend(), [&currency](const Currency* c2) { return currency == c2; });
    if (res != _currencies.cend())
        _currencies.erase(res);
    endResetModel();
}

void CurrencyModel::removeCurrency(QUuid uid)
{
    beginResetModel();
    auto res = std::find_if(_currencies.cbegin(), _currencies.cend(), [&uid](const Currency* currency) { return currency->getUid() == uid; });
    if (res != _currencies.cend())
        _currencies.erase(res);
    endResetModel();
}

Currency* CurrencyModel::addCurrency(const QString& name, const QString& symbol)
{
    auto newCurrency = new Currency(name, symbol);
    addCurrency(newCurrency);
    return newCurrency;
}

Currency* CurrencyModel::getCurrency(const QString& name) const
{
    auto currencyIt = std::find_if(_currencies.cbegin(), _currencies.cend(), [&name](const Currency* currency) { return currency->getName() == name; });
    // case NOT FOUND
    if (currencyIt == _currencies.end())
        return nullptr;

    return *currencyIt;
}

void CurrencyModel::reset()
{
    beginResetModel();
    _currencies.clear();
    endResetModel();
}

Currency* CurrencyModel::getCurrency(QUuid uid) const
{
    auto currencyIt = std::find_if(_currencies.cbegin(), _currencies.cend(), [&uid](const Currency* currency) { return currency->getUid() == uid; });
    // case NOT FOUND
    if (currencyIt == _currencies.end())
        return nullptr;

    return *currencyIt;
}

bool CurrencyModel::isDirty() const
{
    // TODO implement CurrencyModel::isDirty()
    return false;
}
