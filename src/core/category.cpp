#include "category.hpp"

#include <QJsonArray>
#include <QJsonObject>

Category::Category()
{
    _uid = QUuid::createUuid();
}

Category::Category(QString name) : _name(std::move(name))
{
    _uid = QUuid::createUuid();
}

void Category::addSubCategory(const Category* subCategory)
{
    return _subCategories.append(subCategory);
}

bool Category::removeSubCategory(const Category* subCategory)
{
    return _subCategories.removeOne(subCategory);
}

bool Category::removeSubCategoryByName(const QString& name)
{
    return _subCategories.removeIf([&name](const Category* sc) { return sc != nullptr && sc->getName() == name; });
}

QVector<const Category*> Category::subCategories() const
{
    return _subCategories;
}

void Category::read(const QJsonObject& json)
{
    if (json.contains("uid") && json["uid"].isString())
        _uid = QUuid(json["uid"].toString());

    // check that uid is valid, otherwise generate a new one
    if (_uid.isNull())
        _uid = QUuid::createUuid();

    if (json.contains("name") && json["name"].isString())
        _name = json["name"].toString();

    if (json.contains("subcategories") && json["subcategories"].isArray()) {
        QJsonArray subCategoriesArray = json["subcategories"].toArray();
        for (QJsonValue subCategory: subCategoriesArray) {
            auto subCategoryPtr = new Category;
            subCategoryPtr->read(subCategory.toObject());
            _subCategories.append(subCategoryPtr);
        }
    }
}

void Category::write(QJsonObject& json) const
{
    json["uid"] = _uid.toString();
    json["name"] = _name;

    QJsonArray subCategoriesArray;
    for (const Category* subCategory: _subCategories) {
        QJsonObject subCategoryObject;
        subCategory->write(subCategoryObject);
        subCategoriesArray.append(subCategoryObject);
    }
    json["subcategories"] = subCategoriesArray;
}

const QUuid& Category::getUid() const
{
    return _uid;
}

void Category::setUid(const QUuid& uid)
{
    _uid = uid;
}

const QString& Category::getName() const
{
    return _name;
}

void Category::setName(const QString& name)
{
    _name = name;
}

Category* Category::fromJson(const QJsonObject& json)
{
    if (json.isEmpty())
        return nullptr;

    auto category = new Category;
    category->read(json);
    return category;
}