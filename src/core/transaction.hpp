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

    // Getter & Setter
    [[nodiscard]] const QString &getName() const;
    void setName(const QString &name);
    [[nodiscard]] const QString &getComment() const;
    void setComment(const QString &comment);
    [[nodiscard]] TransactionStatus getTs() const;
    void setTs(TransactionStatus ts);
    [[nodiscard]] const QDate &getTransactionDate() const;
    void setTransactionDate(const QDate &transactionDate);
    [[nodiscard]] const QDate &getValueDate() const;
    void setValueDate(const QDate &valueDate);
    [[nodiscard]] double getAmount() const;
    void setAmount(double amount);

    // public API
    void printToConsole() const;

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
