#ifndef OWNER_H
#define OWNER_H

#include <string>
#include <QString>
#include <QMetaType>
#include "currency.hpp"

class Owner {
public:
    Owner();
    Owner(const QString &name, const Currency &currency, float warningBalance, const QString &comment, bool isHidden);
    ~Owner() = default;

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
    void setId(int id);

private:
    int _id = -1;
    QString _name;
    Currency _currency;
    float _warningBalance = 0;
    QString _comment;
    bool _isHidden = false;
};

Q_DECLARE_METATYPE(Owner)

#endif // OWNER_H
