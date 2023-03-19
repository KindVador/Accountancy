#include "accountmodel.hpp"

#include <utility>

constexpr const int ObjectRole = Qt::UserRole + 1;

AccountModel::AccountModel(QString name) : AbstractModel(std::move(name))
{
}

int AccountModel::rowCount(const QModelIndex& parent) const
{
    return (int) _accounts.count();
}

QVariant AccountModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= _accounts.count() || index.row() < 0)
        return {};

    QVariant v;
    switch (role) {
        case Qt::DisplayRole:
            v.setValue(_accounts.at(index.row())->getDisplayedName());
            break;
        case Qt::UserRole:
            v.setValue(_accounts.at(index.row())->getUid());
            break;
        case ObjectRole:
            v.setValue(_accounts.at(index.row()));
            break;
        default:
            break;
    }
    return v;
}

void AccountModel::addAccount(Account* account)
{
    if (account == nullptr)
        return;

    _accounts.append(account);
}

Account* AccountModel::addAccount(const FinancialInstitution* institution, AccountType type, const Currency* currency,
                                  const QList<const Owner*>& owners, float initialBalance, float warningBalance,
                                  const QString& accountNumber, const QString& comment, bool isIncludedInTotal,
                                  bool isHidden)
{
    auto* newAccount = new Account(institution, type, currency, owners, initialBalance, warningBalance, accountNumber, comment, isIncludedInTotal, isHidden);
    addAccount(newAccount);
    return newAccount;
}

void AccountModel::reset()
{
    beginResetModel();
    _accounts.clear();
    endResetModel();
}

void AccountModel::removeAccount(const QModelIndex& index)
{
    beginResetModel();
    delete _accounts.at(index.row());
    _accounts.removeAt(index.row());
    endResetModel();
}

const QList<Account*>& AccountModel::accounts() const
{
    return _accounts;
}

bool AccountModel::isDirty() const
{
    // TODO implement AccountModel::isDirty()
    return false;
}
