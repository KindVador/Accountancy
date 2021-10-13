#include "ownermodel.hpp"

OwnerModel::OwnerModel() {
}

OwnerModel::~OwnerModel() {
}

void OwnerModel::addOwner(Owner *owner) {
    if (owner == nullptr)
        return;

    _owners.insert(owner->getId(), owner);
}

void OwnerModel::addOwner(const QString &name, const Currency &currency, float warningBalance, const QString &comment,
                          bool isHidden) {
    int lastId = _owners.last()->getId();
    auto* newOwner = new Owner(name, currency, warningBalance, comment, isHidden);
    newOwner->setId(lastId +1 );
    addOwner(newOwner);
}

void OwnerModel::removeOwner(Owner *owner) {

}

void OwnerModel::removeOwner(int id) {

}

Owner *OwnerModel::getOwner(const QString &name) {
    return nullptr;
}

