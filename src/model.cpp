#include "model.hpp"

Model::Model(): _ownerModel(new OwnerModel), _currencyModel(new CurrencyModel) {
}

Model::~Model() {
    delete _ownerModel;
    delete _currencyModel;
}