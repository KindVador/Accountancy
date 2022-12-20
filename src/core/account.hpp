#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QJsonObject>
#include <QList>
#include <QString>
#include <QUuid>

#include "Interfaces/iserializable.hpp"
#include "currency.hpp"
#include "owner.hpp"

class Transaction;
class FinancialInstitution;

enum class AccountType : int
{
    Checking = 0,
    CreditCard,
    Savings,
    Cash,
    Assets,
    Loan,
    Investment
};

static QHash<AccountType, QString> ACCOUNT_TYPE_2_STRING{
        {AccountType::Checking, "Checking"},
        {AccountType::CreditCard, "CreditCard"},
        {AccountType::Savings, "Savings"},
        {AccountType::Cash, "Cash"},
        {AccountType::Assets, "Assets"},
        {AccountType::Loan, "Loan"},
        {AccountType::Investment, "Investment"}};

static QHash<QString, AccountType> STRING_2_ACCOUNT_TYPE{
        {"Checking", AccountType::Checking},
        {"CreditCard", AccountType::CreditCard},
        {"Savings", AccountType::Savings},
        {"Cash", AccountType::Cash},
        {"Assets", AccountType::Assets},
        {"Loan", AccountType::Loan},
        {"Investment", AccountType::Investment}};

class Account : public ISerializable
{
    Q_DISABLE_COPY(Account)

public:
    Account();
    Account(const FinancialInstitution* _institution, AccountType type, Currency* currency, const QList<const Owner*>& owners,
            float initialBalance, float warningBalance, QString accountNumber, QString comment,
            bool isIncludedInTotal, bool isHidden);
    ~Account() override;

    static Account* fromJson(const QJsonObject& json);

    // Getter & Setter
    [[nodiscard]] QUuid getUid() const;
    void setUid(QUuid id);
    [[nodiscard]] const FinancialInstitution* getInstitution() const;
    void setInstitution(const FinancialInstitution* institution);
    [[nodiscard]] const Currency* getCurrency() const;
    void setCurrency(const Currency* currency);
    [[nodiscard]] double getInitialBalance() const;
    void setInitialBalance(double initialBalance);
    [[nodiscard]] double getWarningBalance() const;
    void setWarningBalance(double warningBalance);
    [[nodiscard]] const QString& getComment() const;
    void setComment(const QString& comment);
    [[nodiscard]] const QString& getAccountNumber() const;
    void setAccountNumber(const QString& accountNumber);
    [[nodiscard]] bool isIncludedInTotal() const;
    void setIsIncludedInTotal(bool isIncludedInTotal);
    [[nodiscard]] bool isHidden() const;
    void setIsHidden(bool isHidden);
    [[nodiscard]] AccountType getType() const;
    void setType(AccountType type);

    // public API
    [[nodiscard]] QString getDisplayedName() const;
    [[nodiscard]] QList<QUuid> getOwnersUid() const;
    [[nodiscard]] QList<const Owner*>& getOwners();
    bool addTransaction(Transaction* transaction);
    bool removeTransaction(Transaction* transaction);
    bool removeTransaction(const QUuid& uid);
    bool addTransactions(const QList<Transaction*>& transactions);
    [[nodiscard]] int count() const;
    [[nodiscard]] Transaction* transactionAt(int pos) const;
    void addOwner(const Owner* owner);
    void updateTransactionsBalance();

    // Serialization
    void read(const QJsonObject& json) override;
    void write(QJsonObject& json) const override;

private:
    bool isTransactionRegistered(const Transaction* transaction) const;

private:
    QUuid _uid;
    const FinancialInstitution* _institution = nullptr;
    const Currency* _currency = nullptr;
    QList<const Owner*> _owners;
    double _initialBalance = 0.0;
    double _warningBalance = 0.0;
    QString _accountNumber;
    QString _comment;
    bool _isIncludedInTotal = true;
    bool _isHidden = false;
    AccountType _type = AccountType::Checking;
    QList<Transaction*> _transactions;
};

Q_DECLARE_METATYPE(Account*)

#endif// ACCOUNT_H
