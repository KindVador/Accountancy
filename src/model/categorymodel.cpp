#include "categorymodel.hpp"
#include <iostream>
#include <utility>

#include <QJsonArray>

constexpr const int ObjectRole = Qt::UserRole + 1;

CategoryModel::CategoryModel(QString name) : AbstractModel(std::move(name))
{
}

int CategoryModel::rowCount(const QModelIndex& parent) const
{
    Category* category = nullptr;
    if (!parent.isValid()) {
        category = _rootCategory.get();
    } else {
        category = static_cast<Category*>(parent.internalPointer());
    }

    if (category != nullptr)
        return category->childCount();

    return 0;
}

QVariant CategoryModel::data(const QModelIndex& index, int role) const
{
    qDebug() << "CategoryModel::data() " << index.row() << " " << role;
    if (!index.isValid() || index.row() >= _rootCategory->childCount() || index.row() < 0)
        return {};

    QVariant v;
    auto item = static_cast<Category*>(index.internalPointer());
    switch (role) {
        case Qt::DisplayRole:
            v.setValue(item->getName());
            break;
        case Qt::UserRole:
            v.setValue(item->getUid());
            break;
        case ObjectRole:
            v.setValue(item);
            break;
        default:
            break;
    }

    return v;
}

void CategoryModel::addCategory(Category* category, Category* parent)
{
    if (category == nullptr)
        return;

    beginResetModel();
    if (parent == nullptr) {
        qDebug() << "DEBUG: addCategory: " << category->getName();
        _rootCategory->addSubCategory(category);
    } else {
        qDebug() << "DEBUG: addCategory: " << category->getName() << " to " << parent->getName();
        parent->addSubCategory(category);
    }
    endResetModel();
}

Category* CategoryModel::addCategory(const QString& name, Category* parent)
{
    if (name.isEmpty())
        return nullptr;

    auto category = new Category(name);
    // TODO check if Category already exist in the model
    if (parent == nullptr) {
        addCategory(category);
    } else {
        parent->addSubCategory(category);
    }
    return category;
}

void CategoryModel::removeCategory(const QUuid& uid)
{
    beginResetModel();
    // searching which category need to be removed
    Category* categoryToRemove = category(uid);
    if (categoryToRemove == nullptr) {
        qWarning() << "ERROR: no category found for UID: " << uid.toString();
        return;
    }

    Category* parent = categoryToRemove->parentItem();
    if (parent == nullptr) {
        qWarning() << "ERROR: no parent found for category: " << categoryToRemove->getName();
        return;
    }

    parent->removeSubCategory(categoryToRemove);
    endResetModel();
}

void CategoryModel::removeCategory(const QString& name)
{
    beginResetModel();
    // searching which category need to be removed
    Category* categoryToRemove = category(name);
    if (categoryToRemove == nullptr) {
        qWarning() << "ERROR: no category found for name: " << name;
        return;
    }

    Category* parent = categoryToRemove->parentItem();
    if (parent == nullptr) {
        std::cout << "ERROR: no parent found for category: " << categoryToRemove->getName().toCFString() << std::endl;
        return;
    }

    parent->removeSubCategory(categoryToRemove);
    endResetModel();
}

void CategoryModel::reset()
{
    beginResetModel();
    _rootCategory->removeAllSubCategories();
    endResetModel();
}

Qt::ItemFlags CategoryModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QVariant CategoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QAbstractItemModel::headerData(section, orientation, role);
}

QModelIndex CategoryModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return {};

    Category* parentItem;

    if (!parent.isValid())
        parentItem = _rootCategory.get();
    else
        parentItem = static_cast<Category*>(parent.internalPointer());

    Category* childItem = parentItem->subCategory(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return {};
}

QModelIndex CategoryModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return {};

    auto childItem = static_cast<Category*>(child.internalPointer());
    Category* parentItem = childItem->parentItem();

    if (parentItem == _rootCategory.get())
        return {};

    return createIndex(parentItem->row(), 0, parentItem);
}

int CategoryModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return static_cast<Category*>(parent.internalPointer())->columnCount();
    return _rootCategory->columnCount();
}

Category* CategoryModel::category(const QString& name) const
{
    for (Category* category: _rootCategory->subCategories()) {
        if (category != nullptr && category->getName() == name)
            return category;

        // recursive call to look deeper in the tree
        Category* c = this->category(name);

        if (c != nullptr)
            return c;
    }
    return nullptr;
}

Category* CategoryModel::category(const QUuid& uid) const
{
    for (Category* category: _rootCategory->subCategories()) {
        if (category != nullptr && category->getUid() == uid)
            return category;

        // recursive call to look deeper in the tree
        Category* c = this->category(uid);

        if (c != nullptr)
            return c;
    }
    return nullptr;
}

bool CategoryModel::isDirty() const
{
    // TODO implement CategoryModel::isDirty()
    return false;
}

void CategoryModel::write(QJsonObject& json) const
{
    QJsonArray categories;
    for (int i = 0; i < rowCount(QModelIndex()); ++i) {
        const Category* category = data(index(i, 0, QModelIndex()), ObjectRole).value<Category*>();
        QJsonObject categoryJson;
        category->write(categoryJson);
        categories.append(categoryJson);
    }
    json[getName()] = categories;
}

void CategoryModel::read(const QJsonObject& json)
{
    if (json.contains(getName()) && json[getName()].isArray()) {
        QJsonArray categoriesJsonArray = json[getName()].toArray();
        for (const QJsonValueConstRef& category: qAsConst(categoriesJsonArray))
            addCategory(Category::fromJson(category.toObject()));
    }
}
