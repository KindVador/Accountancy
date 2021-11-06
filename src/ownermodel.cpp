#include "ownermodel.hpp"

constexpr const int ObjectRole = Qt::UserRole + 1;

OwnerModel::OwnerModel() = default;

OwnerModel::~OwnerModel() = default;

int OwnerModel::addOwner(Owner *owner)
{
    if (owner == nullptr)
        return -1;

    _owners.append(owner);
    return owner->getId();
}

int OwnerModel::addOwner(const QString &name, const Currency *currency, float warningBalance, const QString &comment, bool isHidden)
{
    int nextId = -1;
    if (_owners.isEmpty())
        nextId = 0;
    else
        nextId = _owners.last()->getId();

    auto *newOwner = new Owner(name, currency, warningBalance, comment, isHidden);
    newOwner->setId(nextId);
    addOwner(newOwner);

    return nextId;
}

void OwnerModel::removeOwner(Owner *owner)
{

}

void OwnerModel::removeOwner(int id)
{

}

Owner *OwnerModel::getOwner(const QString &name)
{
    return nullptr;
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
