#include "categorymodel.hpp"

constexpr const int ObjectRole = Qt::UserRole + 1;

int CategoryModel::rowCount(const QModelIndex& parent) const
{
    return (int) _categories.size();
}

QVariant CategoryModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= _categories.count() || index.row() < 0)
        return {};

    QVariant v;
    switch (role) {
        case Qt::DisplayRole:
            v.setValue(_categories.at(index.row())->getName());
            break;
        case Qt::UserRole:
            v.setValue(_categories.at(index.row())->getUid());
            break;
        case ObjectRole:
            v.setValue(_categories.at(index.row()));
            break;
        default:
            break;
    }

    return v;
}

void CategoryModel::addCategory(Category* category)
{
    if (category == nullptr)
        return;

    beginResetModel();
    _categories.append(category);
    endResetModel();
}

void CategoryModel::removeCategory(const Category* category)
{
    beginResetModel();
    auto res = std::find_if(_categories.cbegin(), _categories.cend(), [&category](const Category* c2) { return category == c2; });
    if (res != _categories.cend())
        _categories.erase(res);
    endResetModel();
}

void CategoryModel::removeCategory(const QUuid& uid)
{
    beginResetModel();
    auto res = std::find_if(_categories.cbegin(), _categories.cend(), [&uid](const Category* category) { return category->getUid() == uid; });
    if (res != _categories.cend())
        _categories.erase(res);
    endResetModel();
}

Category* CategoryModel::getCategory(const QUuid& uid) const
{
    auto categoryIt = std::find_if(_categories.cbegin(), _categories.cend(), [&uid](const Category* category) { return category->getUid() == uid; });
    // case NOT FOUND
    if (categoryIt == _categories.end())
        return nullptr;

    return *categoryIt;
}

void CategoryModel::reset()
{
    beginResetModel();
    _categories.clear();
    endResetModel();
}
