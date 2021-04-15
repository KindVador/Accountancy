#ifndef OWNER_H
#define OWNER_H

#include <string>
#include <QString>
#include "currency.h"

class Owner
{
public:
    Owner();

private:
    QString name;
    Currency currency;
    float warningBalance = 0;
    QString comment;
    bool isHidden = false;
};

#endif // OWNER_H
