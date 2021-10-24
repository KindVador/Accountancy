#include "ownermodel.hpp"

OwnerModel::OwnerModel() {
}

OwnerModel::~OwnerModel() {
}

int OwnerModel::addOwner(Owner *owner) {
    if (owner == nullptr)
        return -1;

    _owners.insert(owner->getId(), owner);

    return owner->getId();
}

int OwnerModel::addOwner(const QString &name, const Currency &currency, float warningBalance, const QString &comment,
                          bool isHidden) {
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

void OwnerModel::removeOwner(Owner *owner) {

}

void OwnerModel::removeOwner(int id) {

}

Owner *OwnerModel::getOwner(const QString &name) {
    return nullptr;
}

