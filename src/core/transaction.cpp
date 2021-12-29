#include "transaction.hpp"
#include <QDebug>
#include <utility>

void Transaction::printToConsole() const
{
    qDebug() << "Transaction:" << _id << " " << _name << " " << _comment;
}

const QString &Transaction::getName() const
{
    return _name;
}

void Transaction::setName(const QString &name)
{
    _name = name;
}

const QString &Transaction::getComment() const
{
    return _comment;
}

void Transaction::setComment(const QString &comment)
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

const QDate &Transaction::getDate() const
{
    return _date;
}

void Transaction::setDate(const QDate &date)
{
    _date = date;
}

double Transaction::getAmount() const
{
    return _amount;
}

void Transaction::setAmount(double amount)
{
    _amount = amount;
}

void Transaction::read(const QJsonObject &json)
{
    if (json.contains("id") && json["id"].isDouble())
        _id = json["id"].toInt();

    if (json.contains("name") && json["name"].isString())
        _name = json["name"].toString();

    if (json.contains("comment") && json["comment"].isString())
        _comment = json["comment"].toString();

    if (json.contains("status") && json["status"].isString())
        _status = STRING_2_TRANSACTION_STATUS[json["status"].toString()];

    if (json.contains("date") && json["date"].isString())
        _date = QDate::fromString(json["date"].toString(), "dd/MM/yyyy");

    if (json.contains("amount") && json["amount"].isDouble())
        _amount = json["amount"].toDouble();

}

void Transaction::write(QJsonObject &json) const
{
    json["id"] = _id;
    json["name"] = _name;
    json["comment"] = _comment;
    json["status"] = TRANSACTION_STATUS_2_STRING[_status];
    json["date"] = _date.toString("dd/MM/yyyy");
    json["amount"] = _amount;
}

Transaction::Transaction(QString &name, QString &comment, TransactionStatus status, QDate &date, double amount):
             _name(name), _comment(comment), _status(status), _date(date), _amount(amount)
{

}

Transaction::Transaction(QString name, QString comment, TransactionStatus status, QDate date, double amount):
        _name(std::move(name)), _comment(std::move(comment)), _status(status), _date(date), _amount(amount)
{

}

int Transaction::getId() const
{
    return _id;
}
