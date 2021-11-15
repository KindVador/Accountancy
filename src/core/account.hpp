#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>
#include <QList>

#include "currency.hpp"
#include "owner.hpp"

enum AccountType: int {Checking, CreditCard, Savings, Cash, Assets, Loan, Investment};

class Account {
public:
    Account();
    Account(AccountType type, Currency *currency, const Owner *owner, float initialBalance, float warningBalance,
            const QString &accountNumber, const QString &comment, bool isIncludedInTotal, bool isHidden);
    ~Account() = default;

    [[nodiscard]] int getId() const;
    [[nodiscard]] QString getDisplayedName() const;

private:
    int _id = -1;
public:
    void setId(int id);

private:
    Currency* _currency;
    QList<const Owner*> _owners;
    float _initialBalance = 0;
    float _warningBalance = 0;
    QString _accountNumber;
    QString _comment;
    bool _isIncludedInTotal = true;
    bool _isHidden = false;
    AccountType _type = AccountType::Checking;
};

Q_DECLARE_METATYPE(Account*)

#endif // ACCOUNT_H
