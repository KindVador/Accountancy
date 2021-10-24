#ifndef ACCOUNTANCY_MODEL_HPP
#define ACCOUNTANCY_MODEL_HPP

#include "core/owner.hpp"
#include "ownermodel.hpp"
#include "currencymodel.hpp"

class Model {

public:
    explicit Model();
    ~Model();

    // Getters
    OwnerModel *getOwnerModel() const;
    OwnerModel *getOwnerModel();
    CurrencyModel *getCurrencyModel() const;
    CurrencyModel *getCurrencyModel();

    static float balanceForOwner(const Owner *owner);

private:
    OwnerModel* _ownerModel = nullptr;
    CurrencyModel* _currencyModel = nullptr;
};


#endif //ACCOUNTANCY_MODEL_HPP
