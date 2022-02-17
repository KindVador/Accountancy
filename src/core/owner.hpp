#ifndef OWNER_H
#define OWNER_H

#include <string>
#include <QString>
#include <QMetaType>
#include "currency.hpp"

class Owner
{
public:
    Owner() = default;
    Owner(QString &name, double warningBalance, QString &comment, bool isHidden);
    Owner(QString name, double warningBalance, QString comment, bool isHidden);
    ~Owner() = default;

    static Owner *fromJson(const QJsonObject &json);

    // Getter & Setter
    [[nodiscard]] QString getName() const;
    void setName(const QString &value);
    [[nodiscard]] float getWarningBalance() const;
    void setWarningBalance(float value);
    [[nodiscard]] QString getComment() const;
    void setComment(const QString &value);
    [[nodiscard]] bool getIsHidden() const;
    void setIsHidden(bool value);
    [[nodiscard]] int getId() const;
    void setId(int id);

    // Serialization
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

private:
    int _id = -1;
    QString _name;
    double _warningBalance = 0.0;
    QString _comment;
    bool _isHidden = false;
};

Q_DECLARE_METATYPE(Owner*)

#endif // OWNER_H
