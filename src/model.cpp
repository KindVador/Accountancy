#include "model.hpp"

Model::Model(): _ownerModel(new OwnerModel), _currencyModel(new CurrencyModel) {
    Currency euro = Currency();
    euro.setName("Euro");
    euro.setSymbol("€");
    _ownerModel->addOwner("Florian", euro, 0, "", false);
}

Model::~Model() {
    delete _ownerModel;
    delete _currencyModel;
}