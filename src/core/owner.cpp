#include "owner.hpp"

Owner::Owner()
{

}

QString Owner::getName() const
{
    return name;
}

void Owner::setName(const QString &value)
{
    name = value;
}

Currency Owner::getCurrency() const
{
    return currency;
}

void Owner::setCurrency(const Currency &value)
{
    currency = value;
}

float Owner::getWarningBalance() const
{
    return warningBalance;
}

void Owner::setWarningBalance(float value)
{
    warningBalance = value;
}

QString Owner::getComment() const
{
    return comment;
}

void Owner::setComment(const QString &value)
{
    comment = value;
}

bool Owner::getIsHidden() const
{
    return isHidden;
}

void Owner::setIsHidden(bool value)
{
    isHidden = value;
}

int Owner::getId() const
{
    return id;
}

Owner::Owner(const QString &name, const Currency &currency, float warningBalance, const QString &comment, bool isHidden)
        : name(name), currency(currency), warningBalance(warningBalance), comment(comment), isHidden(isHidden) {}
