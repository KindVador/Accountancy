#include "transaction.hpp"
#include <QDebug>

Transaction::Transaction(): _ts(TransactionStatus::Imported)
{

}

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
    return _ts;
}

void Transaction::setStatus(TransactionStatus ts)
{
    _ts = ts;
}

const QDate &Transaction::getTransactionDate() const
{
    return _transactionDate;
}

void Transaction::setTransactionDate(const QDate &transactionDate)
{
    _transactionDate = transactionDate;
}

const QDate &Transaction::getValueDate() const
{
    return _valueDate;
}

void Transaction::setValueDate(const QDate &valueDate)
{
    _valueDate = valueDate;
}

double Transaction::getAmount() const
{
    return _amount;
}

void Transaction::setAmount(double amount)
{
    _amount = amount;
}
