#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "account.hpp"

#include <iostream>
#include <QString>
#include <QDate>

enum class TransactionStatus: int
{
    Planned = 0,
    Created,
    Imported,
    Cleared,
    Locked,
    Cancelled
};

class Transaction
{
public:
    Transaction();
    ~Transaction() = default;

private:
    int _id = -1;
    Account *_accountFrom = nullptr;
    Account *_accountTo = nullptr;
    QString _name;
    QString _comment;
    TransactionStatus ts;
    QDate _transactionDate;
    QDate _valueDate;
    double amount = 0.0;
};

Q_DECLARE_METATYPE(Transaction*)

#endif // TRANSACTION_H
