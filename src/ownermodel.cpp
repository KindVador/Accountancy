#include "ownermodel.hpp"

OwnerModel::OwnerModel() = default;

OwnerModel::~OwnerModel() = default;

int OwnerModel::addOwner(Owner *owner)
{
    if (owner == nullptr)
        return -1;

    _owners.append(owner);
    return owner->getId();
}

int OwnerModel::addOwner(const QString &name, const Currency &currency, float warningBalance, const QString &comment, bool isHidden)
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

    switch (role) {
        case Qt::DisplayRole:
            return QVariant(_owners.at(index.row())->getName());
        case Qt::UserRole:
            // TODO try to return a pointer to Owner object
            break;
        default:
            break;
    }

    return {};
}
