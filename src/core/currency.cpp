#include "currency.hpp"

#include <utility>

Currency::Currency(QString &name, QString &symbol): _name(name), _symbol(symbol)
{
}

Currency::Currency(QString name, QString symbol): _name(std::move(name)), _symbol(std::move(symbol))
{
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

int Currency::getId() const
{
    return _id;
}

void Currency::read(const QJsonObject &json)
{
    if (json.contains("id") && json["id"].isDouble())
        _id = json["id"].toInt();

    if (json.contains("name") && json["name"].isString())
        _name = json["name"].toString();

    if (json.contains("symbol") && json["symbol"].isString())
        _symbol = json["symbol"].toString();
}

void Currency::write(QJsonObject &json) const
{
    json["id"] = _id;
    json["name"] = _name;
    json["symbol"] = _symbol;
}


