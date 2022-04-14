#include "owner.hpp"

Owner::Owner()
{
    _uid = QUuid::createUuid();
}

Owner::Owner(QString &name, double warningBalance, QString &comment, bool isHidden)
        : _name(name), _warningBalance(warningBalance), _comment(comment), _isHidden(isHidden)
{
    _uid = QUuid::createUuid();
}

Owner::Owner(QString name, double warningBalance, QString comment, bool isHidden)
        : _name(std::move(name)), _warningBalance(warningBalance), _comment(std::move(comment)), _isHidden(isHidden)
{
    _uid = QUuid::createUuid();
}

QString Owner::getName() const
{
    return _name;
}

void Owner::setName(const QString &value)
{
    _name = value;
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

QUuid Owner::getUid() const
{
    return _uid;
}

void Owner::setUid(QUuid uid)
{
    _uid = uid;
}

void Owner::read(const QJsonObject &json)
{
    if (json.contains("uid") && json["uid"].isString())
        _uid = QUuid(json["uid"].toString());

    if (json.contains("name") && json["name"].isString())
        _name = json["name"].toString();

    if (json.contains("comment") && json["comment"].isString())
        _comment = json["comment"].toString();

    if (json.contains("warning_balance") && json["warning_balance"].isDouble())
        _warningBalance = json["warning_balance"].toDouble();

    if (json.contains("is_hidden") && json["is_hidden"].isBool())
        _isHidden = json["is_hidden"].toBool();
}

void Owner::write(QJsonObject &json) const
{
    json["uid"] = _uid.toString();
    json["name"] = _name;
    json["comment"] = _comment;
    json["warning_balance"] = _warningBalance;
    json["is_hidden"] = _isHidden;
}

Owner *Owner::fromJson(const QJsonObject &json)
{
    if (json.isEmpty())
        return nullptr;

    auto owner = new Owner;
    owner->read(json);
    return owner;
}
