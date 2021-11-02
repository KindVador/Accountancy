#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>
#include <vector>

#include "currency.hpp"
#include "owner.hpp"

enum AccountType: int {Checking, CreditCard, Savings, Cash, Assets, Loan, Investment};

class Account {
public:
    Account();
    ~Account() = default;

private:
    int id = -1;
    Currency currency;
    std::vector<Owner> owners;
    float initialBalance = 0;
    float warningBalance = 0;
    QString accountNumber;
    QString webSite;
    QString comment;
    bool isIncludedInTotal = true;
    bool isHidden = false;
    AccountType type = AccountType::Checking;
};

#endif // ACCOUNT_H
