#include "account.hpp"

Account::Account() {
}

Account::Account(AccountType type, Currency *currency, const Owner *owner, float initialBalance, float warningBalance,
                 const QString &accountNumber, const QString &comment, bool isIncludedInTotal, bool isHidden) :
                 _currency(currency), _initialBalance(initialBalance), _warningBalance(warningBalance),
                 _accountNumber(accountNumber), _comment(comment), _isIncludedInTotal(isIncludedInTotal),
                 _isHidden(isHidden), _type(type) {
    if (owner != nullptr)
        _owners.append(owner);
}
