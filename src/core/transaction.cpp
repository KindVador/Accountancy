#include "transaction.hpp"
#include "category.hpp"
#include <QDebug>
#include <utility>

Transaction::Transaction()
{
    _uid = QUuid::createUuid();
}

Transaction::Transaction(QString name, QString comment, TransactionStatus status, QDateTime datetime, double amount)
    : _name(std::move(name)), _comment(std::move(comment)), _status(status), _dateTime(std::move(datetime)), _amount(amount)
{
    _uid = QUuid::createUuid();
}

// Copy constructor
Transaction::Transaction(const Transaction& origin) : _name(origin._name), _comment(origin._comment), _status(origin._status), _dateTime(origin._dateTime),
                                                      _amount(origin._amount)
{
    _uid = QUuid::fromString(origin._uid.toString());
    _accountFrom = origin._accountFrom;
    _accountTo = origin._accountTo;
    _category = origin._category;
}

// Move constructor
Transaction::Transaction(Transaction&& origin) noexcept : _name(std::move(origin._name)), _comment(std::move(origin._comment)),
                                                          _status(origin._status), _dateTime(origin._dateTime), _amount(origin._amount)
{
    _uid = QUuid::fromString(origin._uid.toString());
    _accountFrom = origin._accountFrom;
    _accountTo = origin._accountTo;
    _category = origin._category;

    // leave origin object in a state in which it is safe to run the destructor
    origin._accountFrom = nullptr;
    origin._accountTo = nullptr;
    origin._category = nullptr;
}

// Copy-Assignment operator
Transaction& Transaction::operator=(const Transaction& rhs)
{
    _uid = QUuid::fromString(rhs._uid.toString());
    _name = rhs._name;
    _comment = rhs._comment;
    _status = rhs._status;
    _dateTime = rhs._dateTime;
    _amount = rhs._amount;
    _current_balance = rhs._current_balance;
    _accountFrom = rhs._accountFrom;
    _accountTo = rhs._accountTo;
    _category = rhs._category;
    return *this;
}

// Move-Assignment operator
Transaction& Transaction::operator=(Transaction&& rhs) noexcept
{
    // test for self-assignment
    if (this != &rhs) {
        _uid = QUuid::fromString(rhs._uid.toString());
        _name = std::move(rhs._name);
        _comment = std::move(rhs._comment);
        _status = rhs._status;
        _dateTime = rhs._dateTime;
        _amount = rhs._amount;
        _current_balance = rhs._current_balance;
        _accountFrom = rhs._accountFrom;
        _accountTo = rhs._accountTo;
        _category = rhs._category;

        // leave rhs object in a destructible state
        rhs._accountFrom = nullptr;
        rhs._accountTo = nullptr;
        rhs._category = nullptr;
    }
    return *this;
}

void Transaction::printToConsole() const
{
    qDebug() << "Transaction:" << _uid << " " << _name << " " << _comment;
}

const QString& Transaction::getName() const
{
    return _name;
}

void Transaction::setName(const QString& name)
{
    _name = name;
}

const QString& Transaction::getComment() const
{
    return _comment;
}

void Transaction::setComment(const QString& comment)
{
    _comment = comment;
}

TransactionStatus Transaction::getStatus() const
{
    return _status;
}

void Transaction::setStatus(TransactionStatus ts)
{
    _status = ts;
}

const QDateTime& Transaction::getDateTime() const
{
    return _dateTime;
}

void Transaction::setDateTime(const QDateTime& datetime)
{
    _dateTime = datetime;
}

double Transaction::getAmount() const
{
    return _amount;
}

void Transaction::setAmount(double amount)
{
    _amount = amount;
}

void Transaction::read(const QJsonObject& json)
{
    if (json.contains("uid") && json["uid"].isString())
        _uid = QUuid(json["uid"].toString());

    // check that uid is valid, otherwise generate a new one
    if (_uid.isNull())
        _uid = QUuid::createUuid();

    if (json.contains("name") && json["name"].isString())
        _name = json["name"].toString();

    if (json.contains("comment") && json["comment"].isString())
        _comment = json["comment"].toString();

    if (json.contains("status") && json["status"].isString())
        _status = STRING_2_TRANSACTION_STATUS[json["status"].toString()];

    if (json.contains("datetime") && json["datetime"].isString())
        _dateTime = QDateTime::fromString(json["datetime"].toString(), Qt::ISODateWithMs);

    if (json.contains("amount") && json["amount"].isDouble())
        _amount = json["amount"].toDouble();

    if (json.contains("current_balance") && json["current_balance"].isDouble())
        _current_balance = json["current_balance"].toDouble();

    if (json.contains("category") && json["category"].isString()) {
        auto category = new Category;
        category->setUid(QUuid(json["category"].toString()));
        setCategory(category);
    }
}

void Transaction::write(QJsonObject& json) const
{
    json["uid"] = _uid.toString();
    json["name"] = _name;
    json["comment"] = _comment;
    json["status"] = TRANSACTION_STATUS_2_STRING[_status];
    json["datetime"] = _dateTime.toString(Qt::ISODateWithMs);
    json["amount"] = _amount;
    json["current_balance"] = _current_balance;
    if (_category)
        json["category"] = _category->getUid().toString();
}

QUuid Transaction::getUid() const
{
    return _uid;
}

double Transaction::getCurrentBalance() const
{
    return _current_balance;
}

void Transaction::setCurrentBalance(double currentBalance)
{
    _current_balance = currentBalance;
}

inline void swap(Transaction& lhs, Transaction& rhs) noexcept
{
    // swap Functions should call swap, NOT std::swap in order to call swap function if defined in specific type.
    using std::swap;
    swap(lhs._uid, rhs._uid);
    swap(lhs._name, rhs._name);
    swap(lhs._comment, rhs._comment);
    swap(lhs._status, rhs._status);
    swap(lhs._dateTime, rhs._dateTime);
    swap(lhs._amount, rhs._amount);
    swap(lhs._current_balance, rhs._current_balance);
    swap(lhs._accountFrom, rhs._accountFrom);// swap the pointers, not the objects
    swap(lhs._accountTo, rhs._accountTo);    // swap the pointers, not the objects
    swap(lhs._category, rhs._category);      // swap the pointers, not the objects
}

Transaction Transaction::fromJson(const QJsonObject& json)
{
    Transaction t;
    t.read(json);
    return t;
}

bool Transaction::operator<(const Transaction& rhs) const
{
    if (_dateTime == rhs._dateTime)
        return _name < rhs._name;
    else
        return _dateTime < rhs._dateTime;
}

bool Transaction::operator>(const Transaction& rhs) const
{
    return rhs < *this;
}

bool Transaction::operator<=(const Transaction& rhs) const
{
    return !(rhs < *this);
}

bool Transaction::operator>=(const Transaction& rhs) const
{
    return !(*this < rhs);
}

bool operator==(const Transaction& lhs, const Transaction& rhs)
{
    if (lhs._name != rhs._name)
        return false;

    if (lhs._dateTime != rhs._dateTime)
        return false;

    if (lhs._amount != rhs._amount)
        return false;

    if (lhs._comment != rhs._comment)
        return false;

    if (lhs._status != rhs._status)
        return false;

    if (lhs._category != rhs._category)
        return false;

    return true;
}

const Category* Transaction::getCategory() const
{
    return _category;
}

void Transaction::setCategory(const Category* category)
{
    _category = category;
}
