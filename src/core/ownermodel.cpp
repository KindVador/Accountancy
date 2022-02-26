#include "ownermodel.hpp"

constexpr const int ObjectRole = Qt::UserRole + 1;

int OwnerModel::addOwner(Owner *owner)
{
    if (owner == nullptr)
        return -1;
    beginResetModel();
    owner->setId(getLastId());
    _owners.append(owner);
    endResetModel();
    return owner->getId();
}

Owner *OwnerModel::addOwner(const QString &name, float warningBalance, const QString &comment, bool isHidden)
{
    auto *newOwner = new Owner(name, warningBalance, comment, isHidden);
    newOwner->setId(getLastId());
    addOwner(newOwner);
    return newOwner;
}

Owner *OwnerModel::getOwner(const QString &name)
{
    auto ownerIt = std::find_if(_owners.begin(), _owners.end(), [&name](Owner *owner) { return owner->getName() == name; });
    // case NOT FOUND
    if (ownerIt == _owners.end())
        return nullptr;

    return *ownerIt;
}

int OwnerModel::rowCount(const QModelIndex &parent) const
{
    return _owners.count();
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
            v.setValue(_owners.at(index.row())->getId());
            break;
        case ObjectRole:
            v.setValue(_owners.at(index.row()));
            break;
        default:
            break;
    }

    return v;
}

int OwnerModel::getLastId() const {
    if (_owners.isEmpty())
        return 0;
    else
        return _owners.last()->getId() + 1;
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
