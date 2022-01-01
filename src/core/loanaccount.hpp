#ifndef LOANACCOUNT_H
#define LOANACCOUNT_H

#include <QString>
#include <QList>

#include "currency.hpp"
#include "owner.hpp"

class LoanAccount
{
public:
    LoanAccount() = default;
    LoanAccount(Currency *currency, const Owner *owner, float initialBalance, const QString &accountNumber,
                const QString &comment, bool isHidden);
    ~LoanAccount() = default;

private:
    int _id = -1;
    Currency* _currency;
    QList<const Owner*> _owners;
    float _initialBalance = 0;
    QString _accountNumber;
    QString _comment;
    bool _isHidden = false;
};

Q_DECLARE_METATYPE(LoanAccount*)

#endif // LOANACCOUNT_H
