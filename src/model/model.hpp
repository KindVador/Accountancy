#ifndef ACCOUNTANCY_MODEL_HPP
#define ACCOUNTANCY_MODEL_HPP

#include "abstractmodel.hpp"
#include "accountmodel.hpp"
#include "categorymodel.hpp"
#include "core/accountfilter.hpp"
#include "core/owner.hpp"
#include "currencymodel.hpp"
#include "financialinstitutionmodel.hpp"
#include "importconfigmodel.hpp"
#include "ownermodel.hpp"
#include "transactionmodel.hpp"

#include <memory>

class Model : public QObject, public AbstractModel
{
    Q_OBJECT

public:
    // Singleton
    explicit Model(QString name);
    static Model* instance();
    Model(Model& other) = delete;

    ~Model() override = default;

    // AbstractModel
    [[nodiscard]] bool isDirty() const override;
    void reset() override;

    // public API to handle sub models
    /**
     * @brief register a model with its name
     * @param newModel is a pointer to the model to be registered
     * @param name is the name of the new model
     * @return true if new model has been successfully registered otherwise false
     */
    bool registerModel(AbstractModel* newModel, const QString& name);
    /**
     * @brief
     * @param model
     * @return
     */
    bool unregisterModel(AbstractModel* model);
    template<typename T>
    T* getModel(const QString& name);

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

private:
    static const std::unique_ptr<Model> _singleton;
    static const QString _modelVersion;
    QVector<std::unique_ptr<AbstractModel>> _models;
    std::unique_ptr<OwnerModel> _ownerModel = std::make_unique<OwnerModel>("Owner");
    std::unique_ptr<CurrencyModel> _currencyModel = std::make_unique<CurrencyModel>("Currency");
    std::unique_ptr<AccountModel> _accountModel = std::make_unique<AccountModel>("Account");
    std::unique_ptr<FinancialInstitutionModel> _institutionsModel = std::make_unique<FinancialInstitutionModel>("FinancialInstitution");
    std::unique_ptr<AccountFilter> _accountFilteredModel = std::make_unique<AccountFilter>();
    std::unique_ptr<ImportConfigModel> _importConfigModel = std::make_unique<ImportConfigModel>("ImportConfig");
    std::unique_ptr<CategoryModel> _categoryModel = std::make_unique<CategoryModel>("Category");
};

#endif//ACCOUNTANCY_MODEL_HPP