#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <iostream>

#include <QDateTime>
#include <QJsonObject>
#include <QString>
#include <QUuid>

#include "Interfaces/iserializable.hpp"

class Account;
class Category;

enum class TransactionStatus : int
{
    Planned = 0,
    Created,
    Imported,
    Cleared,
    Locked,
    Cancelled
};

static const QHash<TransactionStatus, QString> TRANSACTION_STATUS_2_STRING{
        {TransactionStatus::Planned, "Planned"},
        {TransactionStatus::Created, "Created"},
        {TransactionStatus::Imported, "Imported"},
        {TransactionStatus::Cleared, "Cleared"},
        {TransactionStatus::Locked, "Locked"},
        {TransactionStatus::Cancelled, "Cancelled"}};

static const QHash<QString, TransactionStatus> STRING_2_TRANSACTION_STATUS{
        {"Planned", TransactionStatus::Planned},
        {"Created", TransactionStatus::Created},
        {"Imported", TransactionStatus::Imported},
        {"Cleared", TransactionStatus::Cleared},
        {"Locked", TransactionStatus::Locked},
        {"Cancelled", TransactionStatus::Cancelled}};

class Transaction : public ISerializable
{
public:
    Transaction();
    explicit Transaction(QString name, QString comment, TransactionStatus status, QDateTime datetime, double amount);
    ~Transaction() override = default;
    static Transaction fromJson(const QJsonObject& json);
    // Copy constructor
    Transaction(const Transaction& origin);
    // Move constructor
    Transaction(Transaction&& origin) noexcept;

    friend void swap(Transaction& lhs, Transaction& rhs) noexcept;

    // Operators
    Transaction& operator=(const Transaction& rhs);    // Copy-Assignment Operator
    Transaction& operator=(Transaction&& rhs) noexcept;// Move-Assignment Operator
    bool operator<(const Transaction& rhs) const;
    bool operator>(const Transaction& rhs) const;
    bool operator<=(const Transaction& rhs) const;
    bool operator>=(const Transaction& rhs) const;
    friend bool operator==(const Transaction& lhs, const Transaction& rhs);

    // Getter & Setter
    [[nodiscard]] QUuid getUid() const;
    [[nodiscard]] const QString& getName() const;
    void setName(const QString& name);
    [[nodiscard]] const QString& getComment() const;
    void setComment(const QString& comment);
    [[nodiscard]] TransactionStatus getStatus() const;
    void setStatus(TransactionStatus ts);
    [[nodiscard]] const QDateTime& getDateTime() const;
    void setDateTime(const QDateTime& datetime);
    [[nodiscard]] double getAmount() const;
    void setAmount(double amount);
    [[nodiscard]] double getCurrentBalance() const;
    void setCurrentBalance(double currentBalance);
    [[nodiscard]] const Category* getCategory() const;
    void setCategory(const Category* category);

    // Serialization
    void read(const QJsonObject& json) override;
    void write(QJsonObject& json) const override;

    // public API
    void printToConsole() const;

private:
    QUuid _uid;
    QString _name;
    QString _comment;
    TransactionStatus _status = TransactionStatus::Imported;
    QDateTime _dateTime;
    double _amount = 0.0;
    double _current_balance = 0.0;
    const Account* _accountFrom = nullptr;
    const Account* _accountTo = nullptr;
    const Category* _category = nullptr;
};

Q_DECLARE_METATYPE(Transaction*)

#endif// TRANSACTION_H
