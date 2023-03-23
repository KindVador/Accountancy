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

#include <QHash>

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
     * @return true if new model has been successfully registered otherwise false
     */
    bool registerModel(AbstractModel* newModel);

    /**
     * @brief
     * @param model
     * @return
     */
    bool unregisterModel(AbstractModel* model);

    /**
     * @brief
     * @tparam T
     * @param name
     * @return
     */
    template<typename T>
    T* getModel(const QString& name);

    // TODO fix why template specialization is required for linker
    template<>
    CategoryModel* getModel(const QString& name);
    template<>
    FinancialInstitutionModel* getModel(const QString& name);


    // Getters & Setters
    [[nodiscard]] AccountFilter* getAccountFilter() const;
    void setAccountFilter(AccountModel* model);

    // Serialization
    void write(QJsonObject& json) const override;
    void read(const QJsonObject& json) override;

    void setOwnerFilter(QUuid OwnerUid);
    void setOwnerFilter(const QString& ownerName);
    static float balanceForOwner(const Owner* owner);

private:
    static const std::unique_ptr<Model> _singleton;
    static const QString _modelVersion;
    QHash<QString, AbstractModel*> _models;
    // TODO refactoring to move account filter in controller part
    std::unique_ptr<AccountFilter> _accountFilteredModel = std::make_unique<AccountFilter>();
};

#endif//ACCOUNTANCY_MODEL_HPP