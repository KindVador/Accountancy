#include "category.hpp"
#include "subcategory.hpp"

void Category::addSubCategory(const SubCategory* subCategory)
{
    return _subCategories.append(subCategory);
}

bool Category::removeSubCategory(const SubCategory* subCategory)
{
    return _subCategories.removeOne(subCategory);
}

bool Category::removeSubCategoryByName(const QString& name)
{
    return _subCategories.removeIf([&name](const SubCategory* sc) -> bool { return sc != nullptr && sc->getName() == name; });
}

QVector<const SubCategory*> Category::subCategories() const
{
    return _subCategories;
}

void Category::read(const QJsonObject& json)
{
    // TODO implement Category::read(const QJsonObject& json)
}

void Category::write(QJsonObject& json) const
{
    // TODO implement Category::write(QJsonObject& json)
}
