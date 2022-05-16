#include "ownermodel.hpp"

constexpr const int ObjectRole = Qt::UserRole + 1;

void OwnerModel::addOwner(Owner *owner)
{
    if (owner == nullptr)
        return;

    beginResetModel();
    _owners.append(owner);
    endResetModel();
}

Owner *OwnerModel::addOwner(const QString &name, float warningBalance, const QString &comment, bool isHidden)
{
    auto *newOwner = new Owner(name, warningBalance, comment, isHidden);
    addOwner(newOwner);
    return newOwner;
}

Owner *OwnerModel::getOwner(const QString &name) const
{
    auto ownerIt = std::find_if(_owners.begin(), _owners.end(), [&name](Owner *owner) { return owner->getName() == name; });
    // case NOT FOUND
    if (ownerIt == _owners.end())
        return nullptr;

    return *ownerIt;
}

int OwnerModel::rowCount(const QModelIndex &parent) const
{
    return (int)_owners.count();
}

QVariant OwnerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= _owners.count() || index.row() < 0)
        return {};

    QVariant v;
    switch (role) {
        case Qt::DisplayRole:
            v.setValue(_owners.at(index.row())->getName());
            break;
        case Qt::UserRole:
            v.setValue(_owners.at(index.row())->getUid());
            break;
        case ObjectRole:
            v.setValue(_owners.at(index.row()));
            break;
        default:
            break;
    }

    return v;
}

void OwnerModel::reset()
{
    beginResetModel();
    _owners.clear();
    endResetModel();
}

void OwnerModel::removeOwner(const QModelIndex &index)
{
    beginResetModel();
    delete _owners.at(index.row());
    _owners.removeAt(index.row());
    endResetModel();
}

Owner *OwnerModel::getOwner(QUuid uid) const
{
    auto ownerIt = std::find_if(_owners.begin(), _owners.end(), [&uid](Owner *owner) { return owner->getUid() == uid; });
    // case NOT FOUND
    if (ownerIt == _owners.end())
        return nullptr;

    return *ownerIt;
}