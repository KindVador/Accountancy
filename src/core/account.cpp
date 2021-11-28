#include "account.hpp"

Account::Account()
{
    // TODO
}

Account::Account(AccountType type, Currency *currency, const Owner *owner, float initialBalance, float warningBalance,
                 const QString &accountNumber, const QString &comment, bool isIncludedInTotal, bool isHidden) :
                 _currency(currency), _initialBalance(initialBalance), _warningBalance(warningBalance),
                 _accountNumber(accountNumber), _comment(comment), _isIncludedInTotal(isIncludedInTotal),
                 _isHidden(isHidden), _type(type)
{
    if (owner != nullptr)
        _owners.append(owner);
}

QString Account::getDisplayedName() const
{
    return QString("%1").arg(_accountNumber);
}

int Account::getId() const
{
    return _id;
}

void Account::setId(int id)
{
    _id = id;
}

const QList<const Owner *> &Account::getOwners() const
{
    return _owners;
}

QList<int> Account::getOwnersId() const
{
    if (_owners.isEmpty())
        return {};

    QList<int> ids;
    for (const Owner *owner : qAsConst(_owners)) {
        if (owner != nullptr)
            ids.append(owner->getId());
    }

    return ids;
}

void Account::addTransaction(Transaction *transaction)
{
    if (transaction == nullptr)
        return;

    _transactions.append(transaction);
}

void Account::removeTransaction(Transaction *transaction)
{
    if (transaction == nullptr)
        return;

    _transactions.removeOne(transaction);
}
