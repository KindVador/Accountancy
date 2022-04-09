#include "currencymodel.hpp"

constexpr const int ObjectRole = Qt::UserRole + 1;

int CurrencyModel::rowCount(const QModelIndex &parent) const
{
    return _currencies.size();
}

QVariant CurrencyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= _currencies.count() || index.row() < 0)
        return {};

    QVariant v;
    switch (role) {
        case Qt::DisplayRole:
            v.setValue(_currencies.at(index.row())->getDisplayedName());
            break;
        case Qt::UserRole:
            v.setValue(_currencies.at(index.row())->getId());
            break;
        case ObjectRole:
            v.setValue(_currencies.at(index.row()));
            break;
        default:
            break;
    }

    return v;
}

int CurrencyModel::addCurrency(Currency *currency)
{
    if (currency == nullptr)
        return -1;

    beginResetModel();
    currency->setId(getLastId());
    _currencies.append(currency);
    endResetModel();
    return currency->getId();
}

void CurrencyModel::removeCurrency(Currency *currency)
{
    beginResetModel();
    auto res = std::find_if(_currencies.cbegin(), _currencies.cend(), [&currency] (const Currency *c2){ return currency == c2;});
    if (res != _currencies.cend())
        _currencies.erase(res);
    endResetModel();
}

void CurrencyModel::removeCurrency(int id)
{
    beginResetModel();
    auto res = std::find_if(_currencies.cbegin(), _currencies.cend(), [&id] (const Currency *currency){ return currency->getId() == id;});
    if (res != _currencies.cend())
        _currencies.erase(res);
    endResetModel();
}

Currency *CurrencyModel::addCurrency(const QString &name, const QString &symbol)
{
    auto *newCurrency = new Currency(name, symbol);
    newCurrency->setId(getLastId());
    addCurrency(newCurrency);
    return newCurrency;
}

Currency *CurrencyModel::addCurrency(QString &&name, QString &&symbol)
{
    auto *newCurrency = new Currency(name, symbol);
    newCurrency->setId(getLastId());
    addCurrency(newCurrency);
    return newCurrency;
}

Currency *CurrencyModel::getCurrency(const QString &name) const
{
    auto currencyIt = std::find_if(_currencies.begin(), _currencies.end(), [&name](Currency *currency) { return currency->getName() == name; });
    // case NOT FOUND
    if (currencyIt == _currencies.end())
        return nullptr;

    return *currencyIt;
}

int CurrencyModel::getLastId() const
{
    if (_currencies.isEmpty())
        return 0;
    else
        return _currencies.last()->getId() + 1;
}

void CurrencyModel::reset()
{
    beginResetModel();
    _currencies.clear();
    endResetModel();
}

Currency *CurrencyModel::getCurrency(int id) const
{
    auto currencyIt = std::find_if(_currencies.begin(), _currencies.end(), [&id](Currency *currency) { return currency->getId() == id; });
    // case NOT FOUND
    if (currencyIt == _currencies.end())
        return nullptr;

    return *currencyIt;
}
