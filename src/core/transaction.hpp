#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <iostream>
#include <QString>

#include "account.hpp"

#include <QDate>

enum TransactionStatus: int {Planned, Created, Imported, Cleared, Locked, Cancelled};

class Transaction
{
public:
    Transaction();

private:
    int id = -1;
    Account accountFrom, accountTo;
    QString name, comment;
    TransactionStatus ts;
    QDate transactionDate, valueDate;
    double amount;
};

#endif // TRANSACTION_H
