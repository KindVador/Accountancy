#include "loanaccount.hpp"

LoanAccount::LoanAccount(Currency *currency, const Owner *owner, float initialBalance, const QString &accountNumber,
                         const QString &comment, bool isHidden) :
        _currency(currency), _initialBalance(initialBalance), _accountNumber(accountNumber), _comment(comment),
        _isHidden(isHidden) {
    if (owner != nullptr)
        _owners.append(owner);
}
