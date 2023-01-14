#include "category.hpp"
#include "subcategory.hpp"

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
    return _subCategories.removeIf([&name](const SubCategory* sc) { return sc != nullptr && sc->getName() == name; });
}

QVector<const SubCategory*> Category::subCategories() const
{
    return _subCategories;
}

void Category::read(const QJsonObject& json)
{
    if (json.contains("uid") && json["uid"].isString())
        _uid = QUuid(json["uid"].toString());

    if (json.contains("name") && json["name"].isString())
        _name = json["name"].toString();

    if (json.contains("subCategories") && json["subCategories"].isArray()) {
        QJsonArray subCategoriesArray = json["subCategories"].toArray();
        for (QJsonValue subCategory: subCategoriesArray) {
            auto subCategoryPtr = new SubCategory;
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
    for (const SubCategory* subCategory: _subCategories) {
        QJsonObject subCategoryObject;
        subCategory->write(subCategoryObject);
        subCategoriesArray.append(subCategoryObject);
    }
    json["subCategories"] = subCategoriesArray;
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
