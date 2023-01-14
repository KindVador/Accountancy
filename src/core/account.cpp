#include "account.hpp"
#include "financialinstitution.hpp"

#include <QJsonArray>
#include <algorithm>

// Constructors
Account::Account()
{
    _uid = QUuid::createUuid();
}

Account::Account(const FinancialInstitution* institution, AccountType type, const Currency* currency,
                 const QList<const Owner*>& owners, float initialBalance, float warningBalance, QString accountNumber,
                 QString comment, bool isIncludedInTotal, bool isHidden) : _institution(institution), _currency(currency), _owners(owners), _initialBalance(initialBalance),
                                                                           _warningBalance(warningBalance), _accountNumber(std::move(accountNumber)),
                                                                           _comment(std::move(comment)), _isIncludedInTotal(isIncludedInTotal), _isHidden(isHidden), _type(type)
{
    _uid = QUuid::createUuid();
}

// Destructor
Account::~Account()
{
    qDeleteAll(_transactions);
}

QString Account::getDisplayedName() const
{
    return QString("%1").arg(_accountNumber);
}

QUuid Account::getUid() const
{
    return _uid;
}

void Account::setUid(QUuid uid)
{
    _uid = uid;
}

QList<const Owner*>& Account::getOwners()
{
    return _owners;
}

QList<QUuid> Account::getOwnersUid() const
{
    if (_owners.isEmpty())
        return {};

    QList<QUuid> ids;
    for (const Owner* owner: qAsConst(_owners)) {
        if (owner != nullptr)
            ids.append(owner->getUid());
    }

    return ids;
}

bool Account::addTransaction(Transaction* transaction)
{
    // check if transaction already exist in account
    if (transaction == nullptr || isTransactionRegistered(transaction)) {
        qWarning() << "Null Transaction or Transaction already registered in this account";
        return false;
    }

    _transactions.append(transaction);
    return true;
}

bool Account::removeTransaction(Transaction* transaction)
{
    if (transaction == nullptr)
        return false;

    return _transactions.removeOne(transaction);
}

bool Account::removeTransaction(const QUuid& uid)
{
    if (uid.isNull())
        return false;

    auto res = std::find_if(_transactions.cbegin(), _transactions.cend(), [&uid](const Transaction* t) {
        return t != nullptr && t->getUid() == uid;
    });

    if (res != _transactions.cend())
        return removeTransaction(*res);

    return false;
}

const FinancialInstitution* Account::getInstitution() const
{
    return _institution;
}

void Account::setInstitution(const FinancialInstitution* institution)
{
    _institution = institution;
}

int Account::count() const
{
    return (int) _transactions.count();
}

Transaction* Account::transactionAt(int pos) const
{
    if (pos < 0 || pos > _transactions.count() - 1) {
        qCritical() << "Invalid transaction position, received " << pos << " instead of value between [0:" << _transactions.count() - 1 << "]";
        return nullptr;
    }

    return _transactions.at(pos);
}

const Currency* Account::getCurrency() const
{
    return _currency;
}

void Account::setCurrency(const Currency* currency)
{
    _currency = currency;
}

void Account::read(const QJsonObject& json)
{
    if (json.contains("uid") && json["uid"].isString())
        _uid = QUuid(json["uid"].toString());

    if (json.contains("institution")) {
        auto fi = new FinancialInstitution;
        fi->setUid(QUuid(json["institution"].toString()));
        setInstitution(fi);
    }

    if (json.contains("currency")) {
        auto c = new Currency;
        c->setUid(QUuid(json["currency"].toString()));
        setCurrency(c);
    }

    if (json.contains("owners") && json["owners"].isArray()) {
        QJsonArray ownersArray = json["owners"].toArray();
        for (QJsonValue owner: ownersArray) {
            auto ownerPtr = new Owner;
            ownerPtr->setUid(QUuid(owner.toString()));
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

    if (json.contains("transactions") && json["transactions"].isArray()) {
        QJsonArray transactionsArray = json["transactions"].toArray();
        for (QJsonValue transaction: transactionsArray) {
            auto transactionPtr = new Transaction;
            transactionPtr->read(transaction.toObject());
            _transactions.append(transactionPtr);
        }
    }
}

void Account::write(QJsonObject& json) const
{
    json["uid"] = _uid.toString();

    if (_institution != nullptr)
        json["institution"] = _institution->getUid().toString();

    if (_currency != nullptr)
        json["currency"] = _currency->getUid().toString();

    QJsonArray ownersArray;
    for (const Owner* owner: _owners)
        ownersArray.append(owner->getUid().toString());
    json["owners"] = ownersArray;

    json["initialBalance"] = _initialBalance;
    json["warningBalance"] = _warningBalance;
    json["accountNumber"] = _accountNumber;
    json["comment"] = _comment;
    json["isIncludedInTotal"] = _isIncludedInTotal;
    json["isHidden"] = _isHidden;
    json["type"] = ACCOUNT_TYPE_2_STRING[_type];

    QJsonArray transactionsArray;
    for (const Transaction* transaction: _transactions) {
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

const QString& Account::getComment() const
{
    return _comment;
}

void Account::setComment(const QString& comment)
{
    _comment = comment;
}

const QString& Account::getAccountNumber() const
{
    return _accountNumber;
}

void Account::setAccountNumber(const QString& accountNumber)
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

Account* Account::fromJson(const QJsonObject& json)
{
    if (json.isEmpty())
        return nullptr;

    auto account = new Account;
    account->read(json);
    return account;
}

void Account::addOwner(const Owner* owner)
{
    _owners.append(owner);
}

void Account::updateTransactionsBalance()
{
    // sort transactions by date
    std::sort(_transactions.begin(), _transactions.end(), [](const Transaction* t1, const Transaction* t2) { return t1->getDateTime() <
                                                                                                                    t2->getDateTime(); });

    // update current balance for each transactions
    double previousBalance = _initialBalance;
    for (Transaction* transaction: _transactions) {
        transaction->setCurrentBalance(previousBalance + transaction->getAmount());
        previousBalance = transaction->getCurrentBalance();
    }
}

bool Account::addTransactions(const QList<Transaction*>& transactions)
{
    bool res = true;
    // add transactions to the selected count
    for (Transaction* t: transactions)
        res &= addTransaction(t);

    // update Current Balance for each Transaction
    updateTransactionsBalance();

    return res;
}

bool Account::isTransactionRegistered(const Transaction* transaction) const
{
    if (transaction == nullptr)
        return false;

    return std::any_of(_transactions.cbegin(), _transactions.cend(), [&transaction](const Transaction* t) { return *transaction == *t; });
}
