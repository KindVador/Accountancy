#include "subcategory.hpp"

const QString& SubCategory::getName() const
{
    return _name;
}

void SubCategory::setName(const QString& name)
{
    _name = name;
}
const Category* SubCategory::getParent() const
{
    return _parent;
}
void SubCategory::setParent(const Category* parent)
{
    _parent = parent;
}

void SubCategory::read(const QJsonObject& json)
{
    Category::read(json);
    // TODO implement SubCategory::read(const QJsonObject& json)
}

void SubCategory::write(QJsonObject& json) const
{
    Category::write(json);
    // TODO implement SubCategory::write(QJsonObject& json)
}
