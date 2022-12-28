#include "subcategory.hpp"

#include <QJsonObject>

SubCategory::SubCategory(QString name, const Category* parent)
{
    _uid = QUuid::createUuid();
    _name = std::move(name);
    if (parent != nullptr)
        setParent(parent->getUid());
}

QUuid SubCategory::getParent() const
{
    return _parentUid;
}

void SubCategory::setParent(const QUuid& parent)
{
    _parentUid = parent;
}

void SubCategory::read(const QJsonObject& json)
{
    Category::read(json);

    if (json.contains("parentUid") && json["parentUid"].isString())
        _parentUid = QUuid(json["parentUid"].toString());
}

void SubCategory::write(QJsonObject& json) const
{
    Category::write(json);
    json["parentUid"] = _parentUid.toString();
}
