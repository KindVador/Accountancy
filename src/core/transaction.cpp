#include "transaction.hpp"
#include <QDebug>

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

double Transaction::getAmount() const
{
    return _amount;
}

void Transaction::setAmount(double amount)
{
    _amount = amount;
}
