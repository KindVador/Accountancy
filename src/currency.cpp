#include "currency.h"

Currency::Currency()
{

}


Currency::Currency(QString &name, char &symbol)
{
    this->name = name;
    this->symbol = symbol;
}


Currency::~Currency()
{

}
