#ifndef LOANACCOUNT_H
#define LOANACCOUNT_H

#include <QList>
#include <QString>
#include <QUuid>

#include "currency.hpp"
#include "owner.hpp"

class LoanAccount
{
public:
    LoanAccount();
    LoanAccount(Currency* currency, const Owner* owner, float initialBalance, QString accountNumber,
                QString comment, bool isHidden);
    ~LoanAccount() = default;

private:
    QUuid _uid;
    Currency* _currency = nullptr;
    QList<const Owner*> _owners;
    float _initialBalance = 0;
    QString _accountNumber;
    QString _comment;
    bool _isHidden = false;
};

Q_DECLARE_METATYPE(LoanAccount*)

#endif// LOANACCOUNT_H
