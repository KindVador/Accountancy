#include "category.hpp"

#include <QJsonArray>
#include <QJsonObject>

Category::Category() : _name(""), _parentItem(nullptr)
{
    _uid = QUuid::createUuid();
}

Category::Category(QString name, Category* parent) : _name(std::move(name)), _parentItem(parent)
{
    _uid = QUuid::createUuid();
}

void Category::addSubCategory(Category* subCategory)
{
    return _subCategories.append(subCategory);
}

bool Category::removeSubCategory(Category* subCategory)
{
    return _subCategories.removeOne(subCategory);
}

bool Category::removeSubCategoryByName(const QString& name)
{
    return _subCategories.removeIf([&name](const Category* sc) { return sc != nullptr && sc->getName() == name; });
}

QVector<Category*>& Category::subCategories()
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
            subCategoryPtr->setParentItem(this);
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

Category* Category::subCategory(int row) const
{
    if (row < 0 || row >= _subCategories.size())
        return nullptr;
    return _subCategories.at(row);
}

int Category::childCount() const
{
    return static_cast<int>(_subCategories.count());
}

int Category::columnCount() const
{
    return 1;
}

QVariant Category::data(int column) const
{
    if (column == 0)
        return {_name};

    return {};
}

int Category::row() const
{
    if (_parentItem)
        return static_cast<int>(_parentItem->_subCategories.indexOf(const_cast<Category*>(this)));

    return 0;
}

Category* Category::parentItem() const
{
    return _parentItem;
}

void Category::removeAllSubCategories()
{
    for (Category* subCat: _subCategories) {
        subCat->removeAllSubCategories();
    }
    qDeleteAll(_subCategories);
    _subCategories.clear();
}

Category* Category::getParentItem() const
{
    return _parentItem;
}

void Category::setParentItem(Category* parentItem)
{
    _parentItem = parentItem;
}

Category* Category::getCategory(QUuid uid) const
{
    if (_uid == uid)
        return const_cast<Category*>(this);

    for (auto sc: _subCategories) {
        if (sc->getUid() == uid)
            return sc;
        auto sc2 = sc->getCategory(uid);
        if (sc2 != nullptr)
            return sc2;
    }
    return nullptr;
}
