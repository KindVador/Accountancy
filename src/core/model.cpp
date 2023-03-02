#include "model.hpp"
#include <QJsonArray>

const QString Model::_modelVersion = "0.1";

constexpr const int ObjectRole = Qt::UserRole + 1;

const std::unique_ptr<Model> Model::_singleton = std::make_unique<Model>();

Model* Model::instance()
{
    return _singleton.get();
}

Model::Model()
{
    // set source model for AccountFilter
    _accountFilteredModel->setSourceModel(_accountModel.get());
}

OwnerModel* Model::getOwnerModel() const
{
    return _ownerModel.get();
}

CurrencyModel* Model::getCurrencyModel() const
{
    return _currencyModel.get();
}

OwnerModel* Model::getOwnerModel()
{
    return _ownerModel.get();
}

CurrencyModel* Model::getCurrencyModel()
{
    return _currencyModel.get();
}

float Model::balanceForOwner(const Owner* owner)
{
    if (owner == nullptr)
        return 0;

    // TODO: implement this method to compute the Total balance of a Owner
    return -1;
}

AccountModel* Model::getAccountModel() const
{
    return _accountModel.get();
}

AccountModel* Model::getAccountModel()
{
    return _accountModel.get();
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
    const Owner* owner = _ownerModel->getOwner(ownerName);
    _accountFilteredModel->setActiveOwnerUid(owner->getUid());
}

FinancialInstitutionModel* Model::getFinancialInstitutionModel() const
{
    return _institutionsModel.get();
}

FinancialInstitutionModel* Model::getFinancialInstitutionModel()
{
    return _institutionsModel.get();
}

void Model::write(QJsonObject& json) const
{
    // Model version
    json["model_version"] = _modelVersion;

    // Categories
    if (_categoryModel != nullptr) {
        QJsonArray categories;
        for (int i = 0; i < _categoryModel->rowCount(QModelIndex()); ++i) {
            const Category* category = _categoryModel->data(_categoryModel->index(i, 0), ObjectRole).value<Category*>();
            QJsonObject categoryJson;
            category->write(categoryJson);
            categories.append(categoryJson);
        }
        json["categories"] = categories;
    }

    // Currencies
    if (_currencyModel != nullptr) {
        QJsonArray currencies;
        for (int i = 0; i < _currencyModel->rowCount(QModelIndex()); ++i) {
            const Currency* currency = _currencyModel->data(_currencyModel->index(i, 0), ObjectRole).value<Currency*>();
            QJsonObject currencyJson;
            currency->write(currencyJson);
            currencies.append(currencyJson);
        }
        json["currencies"] = currencies;
    }

    // Owners
    if (_ownerModel != nullptr) {
        QJsonArray owners;
        for (int i = 0; i < _ownerModel->rowCount(QModelIndex()); ++i) {
            const Owner* owner = _ownerModel->data(_ownerModel->index(i, 0), ObjectRole).value<Owner*>();
            QJsonObject ownerJson;
            owner->write(ownerJson);
            owners.append(ownerJson);
        }
        json["owners"] = owners;
    }

    // Institutions
    if (_institutionsModel != nullptr) {
        QJsonArray institutions;
        for (int i = 0; i < _institutionsModel->rowCount(QModelIndex()); ++i) {
            const FinancialInstitution* institution = _institutionsModel->data(_institutionsModel->index(i, 0), ObjectRole).value<FinancialInstitution*>();
            QJsonObject institutionJson;
            institution->write(institutionJson);
            institutions.append(institutionJson);
        }
        json["institutions"] = institutions;
    }

    // Accounts
    if (_accountModel != nullptr) {
        QJsonArray accounts;
        for (int i = 0; i < _accountModel->rowCount(QModelIndex()); ++i) {
            const Account* account = _accountModel->data(_accountModel->index(i, 0), ObjectRole).value<Account*>();
            QJsonObject accountJson;
            account->write(accountJson);
            accounts.append(accountJson);
        }
        json["accounts"] = accounts;
    }

    // Import Configs
    if (_importConfigModel != nullptr) {
        QJsonArray importConfigs;
        for (int i = 0; i < _importConfigModel->rowCount(QModelIndex()); ++i) {
            const ImportConfig* importConfig = _importConfigModel->data(_importConfigModel->index(i, 0), ObjectRole).value<ImportConfig*>();
            QJsonObject importConfigJson;
            importConfig->write(importConfigJson);
            importConfigs.append(importConfigJson);
        }
        json["importConfigurations"] = importConfigs;
    }
}

void Model::read(const QJsonObject& json)
{
    // check Model version
    if (!json.contains("model_version") || json["model_version"].toString() != _modelVersion) {
        qWarning() << "Wrong model version";
        return;
    }

    // Owners
    if (json.contains("owners") && json["owners"].isArray()) {
        QJsonArray ownersJsonArray = json["owners"].toArray();
        for (const QJsonValueConstRef& owner: qAsConst(ownersJsonArray))
            _ownerModel->addOwner(Owner::fromJson(owner.toObject()));
    }

    // Categories
    if (json.contains("categories") && json["categories"].isArray()) {
        QJsonArray categoriesJsonArray = json["categories"].toArray();
        for (const QJsonValueConstRef& category: qAsConst(categoriesJsonArray))
            _categoryModel->addCategory(Category::fromJson(category.toObject()));
    }

    // Currencies
    if (json.contains("currencies") && json["currencies"].isArray()) {
        QJsonArray currenciesJsonArray = json["currencies"].toArray();
        for (const QJsonValueConstRef& currency: qAsConst(currenciesJsonArray))
            _currencyModel->addCurrency(Currency::fromJson(currency.toObject()));
    }

    // Financial Institutions
    if (json.contains("institutions") && json["institutions"].isArray()) {
        QJsonArray institutionsJsonArray = json["institutions"].toArray();
        for (const QJsonValueConstRef& institution: qAsConst(institutionsJsonArray))
            _institutionsModel->addFinancialInstitution(FinancialInstitution::fromJson(institution.toObject()));
    }

    // Accounts
    if (json.contains("accounts") && json["accounts"].isArray()) {
        QJsonArray accountsJsonArray = json["accounts"].toArray();
        for (const QJsonValueConstRef& account: qAsConst(accountsJsonArray))
            _accountModel->addAccount(Account::fromJson(account.toObject()));
    }
    // link model objects to each account
    for (Account* account: _accountModel->accounts()) {
        // link currency
        QUuid currencyUid = account->getCurrency()->getUid();
        account->setCurrency(_currencyModel->getCurrency(currencyUid));
        // link financial institution
        QUuid institutionUid = account->getInstitution()->getUid();
        account->setInstitution(_institutionsModel->getFinancialInstitution(institutionUid));
        // link owners
        QList<QUuid> ownersIds = account->getOwnersUid();
        account->getOwners().clear();
        for (QUuid ownerUid: ownersIds)
            account->addOwner(_ownerModel->getOwner(ownerUid));
    }

    // Import Configs
    if (json.contains("importConfigurations") && json["importConfigurations"].isArray()) {
        QJsonArray importConfigsJsonArray = json["importConfigurations"].toArray();
        for (const QJsonValueConstRef& importConfig: qAsConst(importConfigsJsonArray))
            _importConfigModel->addImportConfig(ImportConfig::fromJson(importConfig.toObject()));
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
    if (_ownerModel != nullptr)
        _ownerModel->reset();

    if (_currencyModel != nullptr)
        _currencyModel->reset();

    if (_accountModel != nullptr)
        _accountModel->reset();

    if (_institutionsModel != nullptr)
        _institutionsModel->reset();

    if (_accountFilteredModel != nullptr)
        _accountFilteredModel->invalidate();

    if (_importConfigModel != nullptr)
        _importConfigModel->reset();

    if (_categoryModel != nullptr)
        _categoryModel->reset();
}

ImportConfigModel* Model::getImportConfigModel()
{
    return _importConfigModel.get();
}

ImportConfigModel* Model::getImportConfigModel() const
{
    return _importConfigModel.get();
}
CategoryModel* Model::getCategoryModel()
{
    return _categoryModel.get();
}
