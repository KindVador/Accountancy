#include "financialinstitution.hpp"

#include <utility>

FinancialInstitution::FinancialInstitution(QString name): _name(std::move(name))
{
}

QList<Transaction *> FinancialInstitution::dataFileToTransactions(const QFile &inFile)
{
    // TODO
    return {nullptr};
}

int FinancialInstitution::getId() const
{
    return _id;
}

void FinancialInstitution::setId(int id)
{
    _id = id;
}

const QString &FinancialInstitution::getName() const
{
    return _name;
}

void FinancialInstitution::setName(const QString &name)
{
    _name = name;
}
