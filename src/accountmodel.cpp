#include "accountmodel.hpp"

AccountModel::AccountModel()
{

}

int AccountModel::rowCount(const QModelIndex &parent) const
{
    return 0;
}

QVariant AccountModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

int AccountModel::addAccount(Account *account)
{
    return 0;
}

int AccountModel::addAccount(AccountType type, Currency *currency, const Owner *owner, float initialBalance,
                             float warningBalance, const QString &accountNumber, const QString &comment,
                             bool isIncludedInTotal, bool isHidden) {
    return 0;
}

void AccountModel::removeAccount(Account *account)
{

}

void AccountModel::removeAccount(int id)
{

}

Account *AccountModel::getAccount(const QString &name)
{
    return nullptr;
}

void AccountModel::setFilterOnOwner(int ownerId)
{

}
