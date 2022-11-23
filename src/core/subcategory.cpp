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
void SubCategory::setParent1(const Category* parent)
{
    _parent = parent;
}
