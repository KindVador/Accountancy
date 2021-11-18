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
    void setId(int id);
    [[nodiscard]] QString getDisplayedName() const;
    [[nodiscard]] QList<int> getOwnersId() const;
    [[nodiscard]] const QList<const Owner *> &getOwners() const;

private:
    int _id = -1;
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
