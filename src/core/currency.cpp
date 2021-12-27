#include "currency.hpp"

Currency::Currency(QString &name, QString &symbol)
{
    this->name = name;
    this->symbol = symbol;
}

QString Currency::getName() const
{
    return name;
}

void Currency::setName(const QString &value)
{
    name = value;
}

QString Currency::getSymbol() const
{
    return symbol;
}

void Currency::setSymbol(QString value)
{
    symbol = value;
}

int Currency::getId() const
{
    return id;
}
