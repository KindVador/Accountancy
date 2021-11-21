#ifndef ACCOUNTANCY_MODEL_HPP
#define ACCOUNTANCY_MODEL_HPP

#include "core/owner.hpp"
#include "ownermodel.hpp"
#include "currencymodel.hpp"
#include "accountmodel.hpp"
#include "accountfilter.hpp"

class Model {

public:
    explicit Model();
    ~Model();

    // Getters
    [[nodiscard]] OwnerModel *getOwnerModel() const;
    [[nodiscard]] OwnerModel *getOwnerModel();
    [[nodiscard]] CurrencyModel *getCurrencyModel() const;
    [[nodiscard]] CurrencyModel *getCurrencyModel();
    [[nodiscard]] AccountModel *getAccountModel() const;
    [[nodiscard]] AccountModel *getAccountModel();
    [[nodiscard]] AccountFilter *getAccountFilter() const;

    void setOwnerFilter(int OwnerId);
    static float balanceForOwner(const Owner *owner);

private:
    OwnerModel *_ownerModel = nullptr;
    CurrencyModel *_currencyModel = nullptr;
    AccountModel *_accountModel = nullptr;
    AccountFilter *_accountFilteredModel = nullptr;
};

#endif //ACCOUNTANCY_MODEL_HPP