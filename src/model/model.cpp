#include "model.hpp"
#include <QJsonArray>
#include <utility>

const QString Model::_modelVersion = "0.1";

constexpr const int ObjectRole = Qt::UserRole + 1;

const std::unique_ptr<Model> Model::_singleton = std::make_unique<Model>("Model");

Model* Model::instance()
{
    return _singleton.get();
}

Model::Model(QString name) : AbstractModel(std::move(name))
{
    // set source model for AccountFilter
    _accountFilteredModel->setSourceModel(getModel<AccountModel>("AccountModel"));
}

float Model::balanceForOwner(const Owner* owner)
{
    if (owner == nullptr)
        return 0;

    // TODO: implement this method to compute the Total balance of a Owner
    return -1;
}

AccountFilter* Model::getAccountFilter() const
{
    qWarning() << "Model::getAccountFilter";
    return _accountFilteredModel.get();
}

void Model::setOwnerFilter(QUuid OwnerUid)
{
    if (_accountFilteredModel == nullptr || OwnerUid.isNull())
        return;

    qWarning() << "Model::setOwnerFilter" << OwnerUid;
    _accountFilteredModel->setActiveOwnerUid(OwnerUid);
}

void Model::setOwnerFilter(const QString& ownerName)
{
    if (_accountFilteredModel == nullptr)
        return;

    qWarning() << "Model::setOwnerFilter" << ownerName;
    auto ownerModel = getModel<OwnerModel>("OwnerModel");
    const Owner* owner = ownerModel->getOwner(ownerName);
    _accountFilteredModel->setActiveOwnerUid(owner->getUid());
}

void Model::write(QJsonObject& json) const
{
    // Model version
    json["model_version"] = _modelVersion;

    // write each models
    QHashIterator<QString, AbstractModel*> it(_models);
    while (it.hasNext()) {
        it.next();
        if (it.value() != nullptr) {
            it.value()->write(json);
        }
    }
}

void Model::read(const QJsonObject& json)
{
    qDebug() << "Reading model from JSON object";
    // check Model version
    if (!json.contains("model_version") || json["model_version"].toString() != _modelVersion) {
        qWarning() << "Wrong model version";
        return;
    }

    // read each models
    QHashIterator<QString, AbstractModel*> it(_models);
    while (it.hasNext()) {
        it.next();
        if (it.value() != nullptr) {
            it.value()->read(json);
        }
    }

    // link model objects to each account
    auto accountModel = getModel<AccountModel>("AccountModel");
    auto currencyModel = getModel<CurrencyModel>("CurrencyModel");
    auto institutionsModel = getModel<FinancialInstitutionModel>("FinancialInstitutionModel");
    auto ownerModel = getModel<OwnerModel>("OwnerModel");
    if (accountModel != nullptr && currencyModel != nullptr && institutionsModel != nullptr && ownerModel != nullptr) {
        for (Account* account: accountModel->accounts()) {
            // link currency
            QUuid currencyUid = account->getCurrency()->getUid();
            account->setCurrency(currencyModel->getCurrency(currencyUid));
            // link financial institution
            QUuid institutionUid = account->getInstitution()->getUid();
            account->setInstitution(institutionsModel->getFinancialInstitution(institutionUid));
            // link owners
            QList<QUuid> ownersIds = account->getOwnersUid();
            account->getOwners().clear();
            for (QUuid ownerUid: ownersIds)
                account->addOwner(ownerModel->getOwner(ownerUid));
        }
    }
}

/*!
 * Check if model has unsaved modifications.
 * @return true if model has unsaved modification, otherwise false.
 */
bool Model::isDirty() const
{
    // TODO: check if model has unsaved modifications
    return false;
}

void Model::reset()
{
    if (_accountFilteredModel != nullptr)
        _accountFilteredModel->invalidate();

    QHashIterator<QString, AbstractModel*> it(_models);
    while (it.hasNext()) {
        it.next();
        if (it.value() != nullptr) {
            it.value()->reset();
        }
    }
}

bool Model::registerModel(AbstractModel* newModel)
{
    if (newModel == nullptr) {
        qWarning() << "Invalid model to register";
        return false;
    }

    if (_models.contains(newModel->getName())) {
        qWarning() << "Model : " << newModel->getName() << " already registered";
        return false;
    }

    qDebug() << "Registering model: " << newModel->getName();
    _models.insert(newModel->getName(), newModel);
    return true;
}

bool Model::unregisterModel(AbstractModel* model)
{
    if (model == nullptr)
        return false;

    return _models.remove(model->getName());
}

template<typename T>
T* Model::getModel(const QString& name)
{
    qDebug() << "Model::getModel() " << name;
    if (name.isEmpty() || !_models.contains(name))
        return nullptr;

    auto model = dynamic_cast<T*>(_models.value(name));
    if (model == nullptr)
        qCritical() << "No valid model to return for name: " << name;

    return model;
}

template<>
CategoryModel* Model::getModel(const QString& name)
{
    qDebug() << "Model::getModel() (specialization CategoryModel)" << name;
    if (name.isEmpty() || !_models.contains(name))
        return nullptr;

    return dynamic_cast<CategoryModel*>(_models.value(name));
}

template<>
FinancialInstitutionModel* Model::getModel(const QString& name)
{
    qDebug() << "Model::getModel() (specialization FinancialInstitutionModel)" << name;
    if (name.isEmpty() || !_models.contains(name))
        return nullptr;

    return dynamic_cast<FinancialInstitutionModel*>(_models.value(name));
}