#include "loanaccount.hpp"

#include <utility>

LoanAccount::LoanAccount()
{
    _uid = QUuid::createUuid();
}

LoanAccount::LoanAccount(Currency* currency, const Owner* owner, float initialBalance, QString accountNumber,
                         QString comment, bool isHidden) : _currency(currency), _initialBalance(initialBalance), _accountNumber(std::move(accountNumber)),
                                                           _comment(std::move(comment)), _isHidden(isHidden)
{
    if (owner != nullptr)
        _owners.append(owner);

    _uid = QUuid::createUuid();
}

void LoanAccount::read(const QJsonObject& json)
{
    // TODO: implement the method from the Interface
}

void LoanAccount::write(QJsonObject& json) const
{
    // TODO: implement the method from the Interface
}
