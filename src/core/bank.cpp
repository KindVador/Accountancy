#include "bank.hpp"

QList<Transaction *> Bank::dataFileToTransactions(const QFile &inFile)
{
    // TODO
    return {nullptr};
}

int Bank::getId() const
{
    return _id;
}

void Bank::setId(int id)
{
    _id = id;
}

const QString &Bank::getName() const
{
    return _name;
}

void Bank::setName(const QString &name)
{
    _name = name;
}
