#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "account.hpp"

#include <iostream>
#include <QString>
#include <QDate>
#include <QJsonObject>

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

static QHash<QString, TransactionStatus> STRING_2_TRANSACTION_STATUS {
        {"Planned", TransactionStatus::Planned},
        {"Created", TransactionStatus::Created},
        {"Imported", TransactionStatus::Imported},
        {"Cleared", TransactionStatus::Cleared},
        {"Locked", TransactionStatus::Locked},
        {"Cancelled", TransactionStatus::Cancelled}
};

class Transaction
{
public:
    Transaction() = default;
    explicit Transaction(QString &name, QString &comment, TransactionStatus status, QDate &date, double amount);
    explicit Transaction(QString name, QString comment, TransactionStatus status, QDate date, double amount);
    ~Transaction() = default;

    // Getter & Setter
    [[nodiscard]] const QString &getName() const;
    void setName(const QString &name);
    [[nodiscard]] const QString &getComment() const;
    void setComment(const QString &comment);
    [[nodiscard]] TransactionStatus getStatus() const;
    void setStatus(TransactionStatus ts);
    [[nodiscard]] const QDate &getDate() const;
    void setDate(const QDate &date);
    [[nodiscard]] double getAmount() const;
    void setAmount(double amount);

    // Serialization
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    // public API
    void printToConsole() const;

private:
    int _id = -1;
    QString _name;
    QString _comment;
    TransactionStatus _status = TransactionStatus::Imported;
    QDate _date;
    double _amount = 0.0;
    Account *_accountFrom = nullptr;
    Account *_accountTo = nullptr;
};

Q_DECLARE_METATYPE(Transaction*)

#endif // TRANSACTION_H
