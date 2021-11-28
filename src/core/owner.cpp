#include "owner.hpp"

Owner::Owner()
{
    // TODO
}

Owner::Owner(const QString &name, const Currency *currency, float warningBalance, const QString &comment, bool isHidden)
        : _name(name), _currency(currency), _warningBalance(warningBalance), _comment(comment), _isHidden(isHidden)
{
}

QString Owner::getName() const
{
    return _name;
}

void Owner::setName(const QString &value)
{
    _name = value;
}

const Currency* Owner::getCurrency() const
{
    return _currency;
}

void Owner::setCurrency(const Currency *value)
{
    _currency = value;
}

float Owner::getWarningBalance() const
{
    return _warningBalance;
}

void Owner::setWarningBalance(float value)
{
    _warningBalance = value;
}

QString Owner::getComment() const
{
    return _comment;
}

void Owner::setComment(const QString &value)
{
    _comment = value;
}

bool Owner::getIsHidden() const
{
    return _isHidden;
}

void Owner::setIsHidden(bool value)
{
    _isHidden = value;
}

int Owner::getId() const
{
    return _id;
}

void Owner::setId(int id)
{
    _id = id;
}
