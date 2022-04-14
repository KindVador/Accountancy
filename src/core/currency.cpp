#include "currency.hpp"

#include <utility>

Currency::Currency()
{
    _uid = QUuid::createUuid();
}

Currency::Currency(QString name, QString symbol): _name(std::move(name)), _symbol(std::move(symbol))
{
    _uid = QUuid::createUuid();
}

QString Currency::getName() const
{
    return _name;
}

void Currency::setName(const QString &value)
{
    _name = value;
}

QString Currency::getSymbol() const
{
    return _symbol;
}

void Currency::setSymbol(QString value)
{
    _symbol = std::move(value);
}

QUuid Currency::getUid() const
{
    return _uid;
}

void Currency::read(const QJsonObject &json)
{
    if (json.contains("uid") && json["uid"].isString())
        _uid = QUuid(json["uid"].toString());

    if (json.contains("name") && json["name"].isString())
        _name = json["name"].toString();

    if (json.contains("symbol") && json["symbol"].isString())
        _symbol = json["symbol"].toString();
}

void Currency::write(QJsonObject &json) const
{
    json["uid"] = _uid.toString();
    json["name"] = _name;
    json["symbol"] = _symbol;
}

void Currency::setUid(QUuid uid)
{
    _uid = uid;
}

Currency *Currency::fromJson(const QJsonObject &json)
{
    if (json.isEmpty())
        return nullptr;

    auto currency = new Currency;
    currency->read(json);
    return currency;
}

QString Currency::getDisplayedName() const
{
    return QString("%1 (%2)").arg(_name, _symbol);
}
