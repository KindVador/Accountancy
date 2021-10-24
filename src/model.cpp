#include "model.hpp"

Model::Model(): _ownerModel(new OwnerModel), _currencyModel(new CurrencyModel) {
}

Model::~Model() {
    delete _ownerModel;
    delete _currencyModel;
}

OwnerModel *Model::getOwnerModel() const {
    return _ownerModel;
}

CurrencyModel *Model::getCurrencyModel() const {
    return _currencyModel;
}

OwnerModel *Model::getOwnerModel() {
    return _ownerModel;
}

CurrencyModel *Model::getCurrencyModel() {
    return _currencyModel;
}

float Model::balanceForOwner(const Owner *owner) {
    return 0;
}
