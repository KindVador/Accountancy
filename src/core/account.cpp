#include "account.hpp"
#include "financialinstitution.hpp"

#include <QJsonArray>

Account::Account(const FinancialInstitution *institution, AccountType type, Currency *currency,
                 const QList<const Owner*> &owners, float initialBalance, float warningBalance, QString accountNumber,
                 QString comment, bool isIncludedInTotal, bool isHidden) :
                 _institution(institution), _currency(currency), _owners(owners), _initialBalance(initialBalance),
                 _warningBalance(warningBalance), _accountNumber(std::move(accountNumber)),
                 _comment(std::move(comment)), _isIncludedInTotal(isIncludedInTotal), _isHidden(isHidden), _type(type)
{
}

QString Account::getDisplayedName() const
{
    return QString("%1").arg(_accountNumber);
}

int Account::getId() const
{
    return _id;
}

void Account::setId(int id)
{
    _id = id;
}

const QList<const Owner *> &Account::getOwners() const
{
    return _owners;
}

QList<int> Account::getOwnersId() const
{
    if (_owners.isEmpty())
        return {};

    QList<int> ids;
    for (const Owner *owner : qAsConst(_owners)) {
        if (owner != nullptr)
            ids.append(owner->getId());
    }

    return ids;
}

void Account::addTransaction(Transaction *transaction)
{
    if (transaction == nullptr)
        return;

    _transactions.append(transaction);
}

void Account::removeTransaction(Transaction *transaction)
{
    if (transaction == nullptr)
        return;

    _transactions.removeOne(transaction);
}

const FinancialInstitution *Account::getInstitution() const
{
    return _institution;
}

void Account::setInstitution(const FinancialInstitution *institution)
{
    _institution = institution;
}

int Account::count() const
{
    return _transactions.count();
}

Transaction *Account::transactionAt(int pos) const
{
    return _transactions.at(pos);
}

const Currency *Account::getCurrency() const
{
    return _currency;
}

void Account::setCurrency(const Currency *currency)
{
    _currency = currency;
}

void Account::read(const QJsonObject &json)
{
    if (json.contains("id") && json["id"].isDouble())
        _id = json["id"].toInt();

    if (json.contains("institution")) {
        auto fi = new FinancialInstitution;
        fi->setId(json["institution"].toInt());
        setInstitution(fi);
    }

    if (json.contains("currency")) {
        auto  c = new Currency;
        c->setId(json["currency"].toInt());
        setCurrency(c);
    }

    if (json.contains("owners") && json["owners"].isArray()) {
        QJsonArray ownersArray = json["owners"].toArray();
        for (QJsonValue owner : ownersArray) {
            auto ownerPtr = new Owner;
            ownerPtr->setId(owner.toInt());
            _owners.append(ownerPtr);
        }
    }

    if (json.contains("initialBalance") && json["initialBalance"].isDouble())
        _initialBalance = json["initialBalance"].toDouble();

    if (json.contains("warningBalance") && json["warningBalance"].isDouble())
        _warningBalance = json["warningBalance"].toDouble();

    if (json.contains("accountNumber") && json["accountNumber"].isString())
        _accountNumber = json["accountNumber"].toString();

    if (json.contains("comment") && json["comment"].isString())
        _comment = json["comment"].toString();

    if (json.contains("isIncludedInTotal") && json["isIncludedInTotal"].isBool())
        _isIncludedInTotal = json["isIncludedInTotal"].toBool();

    if (json.contains("isHidden") && json["isHidden"].isBool())
        _isHidden = json["isHidden"].toBool();

    if (json.contains("type") && json["type"].isString())
        _type = STRING_2_ACCOUNT_TYPE[json["type"].toString()];
}

void Account::write(QJsonObject &json) const
{
    json["id"] = _id;

    if (_institution != nullptr)
        json["institution"] = _institution->getId();

    if (_currency != nullptr)
        json["currency"] = _currency->getId();

    QJsonArray ownersArray;
    for (const Owner *owner : _owners)
        ownersArray.append(owner->getId());
    json["owners"] = ownersArray;

    json["initialBalance"] = _initialBalance;
    json["warningBalance"] = _warningBalance;
    json["accountNumber"] = _accountNumber;
    json["comment"] = _comment;
    json["isIncludedInTotal"] = _isIncludedInTotal;
    json["isHidden"] = _isHidden;
    json["type"] = ACCOUNT_TYPE_2_STRING[_type];

    QJsonArray transactionsArray;
    for (const Transaction *transaction : _transactions) {
        QJsonObject transactionObject;
        transaction->write(transactionObject);
        transactionsArray.append(transactionObject);
    }
    json["transactions"] = transactionsArray;
}

double Account::getInitialBalance() const
{
    return _initialBalance;
}

void Account::setInitialBalance(double initialBalance)
{
    _initialBalance = initialBalance;
}

double Account::getWarningBalance() const
{
    return _warningBalance;
}

void Account::setWarningBalance(double warningBalance)
{
    _warningBalance = warningBalance;
}

const QString &Account::getComment() const
{
    return _comment;
}

void Account::setComment(const QString &comment)
{
    _comment = comment;
}

const QString &Account::getAccountNumber() const
{
    return _accountNumber;
}

void Account::setAccountNumber(const QString &accountNumber)
{
    _accountNumber = accountNumber;
}

bool Account::isIncludedInTotal() const
{
    return _isIncludedInTotal;
}

void Account::setIsIncludedInTotal(bool isIncludedInTotal)
{
    _isIncludedInTotal = isIncludedInTotal;
}

bool Account::isHidden() const
{
    return _isHidden;
}

void Account::setIsHidden(bool isHidden)
{
    _isHidden = isHidden;
}

AccountType Account::getType() const
{
    return _type;
}

void Account::setType(AccountType type)
{
    _type = type;
}

Account *Account::fromJson(const QJsonObject &json)
{
    if (json.isEmpty())
        return nullptr;

    auto account = new Account;
    account->read(json);
    return account;
}
