#ifndef OWNER_H
#define OWNER_H

#include <QMetaType>
#include <QString>
#include <QUuid>

#include "Interfaces/iserializable.hpp"
#include "currency.hpp"

class Owner : public ISerializable
{
public:
    Owner();
    Owner(QString& name, double warningBalance, QString& comment, bool isHidden);
    Owner(QString name, double warningBalance, QString comment, bool isHidden);
    ~Owner() override = default;

    static Owner* fromJson(const QJsonObject& json);

    // Getter & Setter
    [[nodiscard]] QString getName() const;
    void setName(const QString& value);
    [[nodiscard]] float getWarningBalance() const;
    void setWarningBalance(float value);
    [[nodiscard]] QString getComment() const;
    void setComment(const QString& value);
    [[nodiscard]] bool getIsHidden() const;
    void setIsHidden(bool value);
    [[nodiscard]] QUuid getUid() const;
    void setUid(QUuid uid);

    // Serialization
    void read(const QJsonObject& json) override;
    void write(QJsonObject& json) const override;

private:
    QUuid _uid;
    QString _name;
    double _warningBalance = 0.0;
    QString _comment;
    bool _isHidden = false;
};

Q_DECLARE_METATYPE(Owner*)

#endif// OWNER_H
