#include "accountmodel.hpp"

constexpr const int ObjectRole = Qt::UserRole + 1;

int AccountModel::rowCount(const QModelIndex &parent) const
{
    return (int) _accounts.count();
}

QVariant AccountModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= _accounts.count() || index.row() < 0)
        return {};

    QVariant v;
    switch (role) {
        case Qt::DisplayRole:
            v.setValue(_accounts.at(index.row())->getDisplayedName());
            break;
        case Qt::UserRole:
            v.setValue(_accounts.at(index.row())->getId());
            break;
        case ObjectRole:
            v.setValue(_accounts.at(index.row()));
            break;
        default:
            break;
    }
    return v;
}

int AccountModel::addAccount(Account *account)
{
    if (account == nullptr)
        return -1;

    int nextId = -1;
    if (_accounts.isEmpty())
        nextId = 0;
    else
        nextId = _accounts.last()->getId() + 1;

    account->setId(nextId);
    _accounts.append(account);
    return nextId;
}

Account *AccountModel::addAccount(const FinancialInstitution *institution, AccountType type, Currency *currency,
                             const Owner *owner, float initialBalance, float warningBalance,
                             const QString &accountNumber, const QString &comment, bool isIncludedInTotal,
                             bool isHidden)
{
    auto *newAccount = new Account(institution, type, currency, owner, initialBalance, warningBalance, accountNumber, comment, isIncludedInTotal, isHidden);
    addAccount(newAccount);
    return newAccount;
}
