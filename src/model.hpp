#ifndef ACCOUNTANCY_MODEL_HPP
#define ACCOUNTANCY_MODEL_HPP

#include "core/owner.hpp"
#include "ownermodel.hpp"
#include "currencymodel.hpp"
#include "accountmodel.hpp"

class Model {

public:
    explicit Model();
    ~Model();

    // Getters
    [[nodiscard]] OwnerModel *getOwnerModel() const;
    [[nodiscard]] OwnerModel *getOwnerModel();
    [[nodiscard]] CurrencyModel *getCurrencyModel() const;
    [[nodiscard]] CurrencyModel *getCurrencyModel();

    static float balanceForOwner(const Owner *owner);

private:
    OwnerModel *_ownerModel = nullptr;
    CurrencyModel *_currencyModel = nullptr;
    AccountModel *_accountModel = nullptr;
};

#endif //ACCOUNTANCY_MODEL_HPP
