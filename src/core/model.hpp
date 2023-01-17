#ifndef ACCOUNTANCY_MODEL_HPP
#define ACCOUNTANCY_MODEL_HPP

#include "accountfilter.hpp"
#include "accountmodel.hpp"
#include "categorymodel.hpp"
#include "currencymodel.hpp"
#include "financialinstitutionmodel.hpp"
#include "importconfigmodel.hpp"
#include "owner.hpp"
#include "ownermodel.hpp"
#include "transactionmodel.hpp"

#include <memory>

class Model : public QObject
{
    Q_OBJECT

public:
    // Singleton
    explicit Model();
    static Model* instance();
    Model(Model& other) = delete;

    ~Model() override = default;

    // Getters
    [[nodiscard]] OwnerModel* getOwnerModel() const;
    [[nodiscard]] OwnerModel* getOwnerModel();
    [[nodiscard]] CurrencyModel* getCurrencyModel() const;
    [[nodiscard]] CurrencyModel* getCurrencyModel();
    [[nodiscard]] AccountModel* getAccountModel() const;
    [[nodiscard]] AccountModel* getAccountModel();
    [[nodiscard]] AccountFilter* getAccountFilter() const;
    [[nodiscard]] FinancialInstitutionModel* getFinancialInstitutionModel() const;
    [[nodiscard]] FinancialInstitutionModel* getFinancialInstitutionModel();
    [[nodiscard]] ImportConfigModel* getImportConfigModel() const;
    [[nodiscard]] ImportConfigModel* getImportConfigModel();
    [[nodiscard]] CategoryModel* getCategoryModel();

    // Serialization
    void write(QJsonObject& json) const;
    void read(const QJsonObject& json);

    void setOwnerFilter(QUuid OwnerUid);
    void setOwnerFilter(const QString& ownerName);
    static float balanceForOwner(const Owner* owner);

    static TransactionModel* getTransactionModel(Account* selectedAccount);

    void reset();
    [[nodiscard]] bool isDirty() const;

private:
    static const std::unique_ptr<Model> _singleton;
    static const QString _modelVersion;
    std::unique_ptr<OwnerModel> _ownerModel = std::make_unique<OwnerModel>();
    std::unique_ptr<CurrencyModel> _currencyModel = std::make_unique<CurrencyModel>();
    std::unique_ptr<AccountModel> _accountModel = std::make_unique<AccountModel>();
    std::unique_ptr<FinancialInstitutionModel> _institutionsModel = std::make_unique<FinancialInstitutionModel>();
    std::unique_ptr<AccountFilter> _accountFilteredModel = std::make_unique<AccountFilter>();
    std::unique_ptr<ImportConfigModel> _importConfigModel = std::make_unique<ImportConfigModel>();
    std::unique_ptr<CategoryModel> _categoryModel = std::make_unique<CategoryModel>();
};

#endif//ACCOUNTANCY_MODEL_HPP