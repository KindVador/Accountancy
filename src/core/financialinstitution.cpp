#include "financialinstitution.hpp"
#include "core/currency.hpp"
#include "importconfig.hpp"

#include <QDebug>
#include <QTextStream>
#include <utility>

FinancialInstitution::FinancialInstitution()
{
    _uid = QUuid::createUuid();
}

FinancialInstitution::FinancialInstitution(QString name) : _name(std::move(name))
{
    _uid = QUuid::createUuid();
}

QUuid FinancialInstitution::getUid() const
{
    return _uid;
}

void FinancialInstitution::setUid(QUuid uid)
{
    _uid = uid;
}

const QString& FinancialInstitution::getName() const
{
    return _name;
}

void FinancialInstitution::setName(const QString& name)
{
    _name = name;
}

void FinancialInstitution::read(const QJsonObject& json)
{
    if (json.contains("uid") && json["uid"].isString())
        _uid = QUuid(json["uid"].toString());

    // check that uid is valid, otherwise generate a new one
    if (_uid.isNull())
        _uid = QUuid::createUuid();

    if (json.contains("name") && json["name"].isString())
        _name = json["name"].toString();
}

void FinancialInstitution::write(QJsonObject& json) const
{
    json["uid"] = _uid.toString();
    json["name"] = _name;
}

FinancialInstitution* FinancialInstitution::fromJson(const QJsonObject& json)
{
    if (json.isEmpty())
        return nullptr;

    auto fi = new FinancialInstitution;
    fi->read(json);
    return fi;
}
