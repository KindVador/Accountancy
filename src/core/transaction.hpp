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

static QHash<TransactionStatus, QString> TRANSACTION_STATUS_2_STRING {
    {TransactionStatus::Planned, "Planned"},
    {TransactionStatus::Created, "Created"},
    {TransactionStatus::Imported, "Imported"},
    {TransactionStatus::Cleared, "Cleared"},
    {TransactionStatus::Locked, "Locked"},
    {TransactionStatus::Cancelled, "Cancelled"}
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
    [[nodiscard]] TransactionStatus getStatus() const;
    void setStatus(TransactionStatus ts);
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
    TransactionStatus _ts;
    QDate _transactionDate;
    QDate _valueDate;
    double _amount = 0.0;
};

Q_DECLARE_METATYPE(Transaction*)

#endif // TRANSACTION_H
