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
    bool unregisterModel(const AbstractModel* model);

    /**
     * @brief template function to get a pointer to a sub model.
     * This function need to be implemented in the header to avoid lack of valid instantiation when compiling other object files,
     * If you put your template definition in a separate .cpp file, when the compiler compiles that file it may not know which instantiations you need.
     * Conversely, at the call sites which would instantiate the correct version of the template function,
     * if the definition of the function body isn't available the compiler won't have the information to instantiate the required specializations.
     * @tparam T
     * @param name
     * @return
     */
    template<typename T>
    inline T* getModel(const QString& name)
    {
        if (name.isEmpty() || !_models.contains(name))
            return nullptr;

        auto model = dynamic_cast<T*>(_models.value(name));
        if (model == nullptr)
            qCritical() << "No valid model to return for name: " << name;

        return model;
    }

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