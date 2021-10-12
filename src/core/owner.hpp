#ifndef OWNER_H
#define OWNER_H

#include <string>
#include <QString>
#include "currency.hpp"

class Owner {
public:
    Owner();
    Owner(const QString &name, const Currency &currency, float warningBalance, const QString &comment, bool isHidden);

    QString getName() const;
    void setName(const QString &value);

    Currency getCurrency() const;
    void setCurrency(const Currency &value);

    float getWarningBalance() const;
    void setWarningBalance(float value);

    QString getComment() const;
    void setComment(const QString &value);

    bool getIsHidden() const;
    void setIsHidden(bool value);

    int getId() const;

private:
    int id = -1;
    QString name;
    Currency currency;
    float warningBalance = 0;
    QString comment;
    bool isHidden = false;
};

#endif // OWNER_H
