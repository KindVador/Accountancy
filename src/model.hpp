#ifndef ACCOUNTANCY_MODEL_HPP
#define ACCOUNTANCY_MODEL_HPP

#include "core/owner.hpp"
#include "ownermodel.hpp"
#include "currencymodel.hpp"
#include "accountmodel.hpp"
#include "accountfilter.hpp"
#include "financialinstitutionmodel.hpp"

class Model : public QObject
{
    Q_OBJECT

public:
    // Singleton
    static Model *getInstance();
    Model(Model &other) = delete;

    ~Model() override;

    // Getters
    [[nodiscard]] OwnerModel *getOwnerModel() const;
    [[nodiscard]] OwnerModel *getOwnerModel();
    [[nodiscard]] CurrencyModel *getCurrencyModel() const;
    [[nodiscard]] CurrencyModel *getCurrencyModel();
    [[nodiscard]] AccountModel *getAccountModel() const;
    [[nodiscard]] AccountModel *getAccountModel();
    [[nodiscard]] AccountFilter *getAccountFilter() const;
    [[nodiscard]] FinancialInstitutionModel *getFinancialInstitutionModel() const;
    [[nodiscard]] FinancialInstitutionModel *getFinancialInstitutionModel();

    // Serialization
    void write(QJsonObject &json) const;

    void setOwnerFilter(int OwnerId);
    void setOwnerFilter(const QString &ownerName);
    static float balanceForOwner(const Owner *owner);

private:
    static Model *_singleton;
    OwnerModel *_ownerModel = nullptr;
    CurrencyModel *_currencyModel = nullptr;
    AccountModel *_accountModel = nullptr;
    AccountFilter *_accountFilteredModel = nullptr;
    FinancialInstitutionModel *_institutionsModel = nullptr;

    // Singleton
    Model();
};

#endif //ACCOUNTANCY_MODEL_HPP