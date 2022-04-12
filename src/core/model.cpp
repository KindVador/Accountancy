#include <QJsonArray>
#include "model.hpp"

const QString Model::_modelVersion = "0.1";

constexpr const int ObjectRole = Qt::UserRole + 1;

Model *Model::_singleton = nullptr;

Model *Model::instance()
{
    if (_singleton == nullptr)
        _singleton = new Model();

    return _singleton;
}

Model::Model(): _ownerModel(new OwnerModel), _currencyModel(new CurrencyModel), _accountModel(new AccountModel),
                _accountFilteredModel(new AccountFilter), _institutionsModel(new FinancialInstitutionModel)
{
    // set source model for AccountFilter
    _accountFilteredModel->setSourceModel(_accountModel);
}

Model::~Model()
{
    delete _ownerModel;
    delete _currencyModel;
    delete _accountModel;
    delete _accountFilteredModel;
    delete _institutionsModel;
}

OwnerModel *Model::getOwnerModel() const
{
    return _ownerModel;
}

CurrencyModel *Model::getCurrencyModel() const
{
    return _currencyModel;
}

OwnerModel *Model::getOwnerModel()
{
    return _ownerModel;
}

CurrencyModel *Model::getCurrencyModel()
{
    return _currencyModel;
}

float Model::balanceForOwner(const Owner *owner)
{
    return 0;
}

AccountModel *Model::getAccountModel() const {
    return _accountModel;
}

AccountModel *Model::getAccountModel() {
    return _accountModel;
}

AccountFilter *Model::getAccountFilter() const
{
    qWarning() << "Model::getAccountFilter";
    return _accountFilteredModel;
}

void Model::setOwnerFilter(QUuid OwnerUid)
{
    if (_accountFilteredModel == nullptr || OwnerUid.isNull())
        return;

    qWarning() << "Model::setOwnerFilter" << OwnerUid;
    _accountFilteredModel->setActiveOwnerUid(OwnerUid);
}

void Model::setOwnerFilter(const QString &ownerName)
{
    if (_accountFilteredModel == nullptr)
        return;

    qWarning() << "Model::setOwnerFilter" << ownerName;
    Owner *owner = _ownerModel->getOwner(ownerName);
    _accountFilteredModel->setActiveOwnerUid(owner->getUid());
}

FinancialInstitutionModel *Model::getFinancialInstitutionModel() const
{
    return _institutionsModel;
}

FinancialInstitutionModel *Model::getFinancialInstitutionModel()
{
    return _institutionsModel;
}

void Model::write(QJsonObject &json) const
{
    // Model version
    json["model_version"] = _modelVersion;

    // Currencies
    if (_currencyModel != nullptr) {
        QJsonArray currencies;
        for (int i = 0; i < _currencyModel->rowCount(QModelIndex()); ++i) {
            auto *currency = _currencyModel->data(_currencyModel->index(i, 0), ObjectRole).value<Currency *>();
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
            auto *owner = _ownerModel->data(_ownerModel->index(i, 0), ObjectRole).value<Owner *>();
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
            auto *institution = _institutionsModel->data(_institutionsModel->index(i, 0), ObjectRole).value<FinancialInstitution *>();
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
            auto *account = _accountModel->data(_accountModel->index(i, 0), ObjectRole).value<Account *>();
            QJsonObject accountJson;
            account->write(accountJson);
            accounts.append(accountJson);
        }
        json["accounts"] = accounts;
    }
}

void Model::read(const QJsonObject &json)
{
    // check Model version
    if (!json.contains("model_version") || json["model_version"].toString() != _modelVersion) {
        qWarning() << "Wrong model version";
        return;
    }

    // Owners
    if (json.contains("owners") && json["owners"].isArray()) {
        QJsonArray ownersJsonArray = json["owners"].toArray();
        for (const QJsonValueRef &owner : qAsConst(ownersJsonArray))
            _ownerModel->addOwner(Owner::fromJson(owner.toObject()));
    }

    // Currencies
    if (json.contains("currencies") && json["currencies"].isArray()) {
        QJsonArray currenciesJsonArray = json["currencies"].toArray();
        for (const QJsonValueRef &currency : qAsConst(currenciesJsonArray))
            _currencyModel->addCurrency(Currency::fromJson(currency.toObject()));
    }

    // Financial Institutions
    if (json.contains("institutions") && json["institutions"].isArray()) {
        QJsonArray institutionsJsonArray = json["institutions"].toArray();
        for (const QJsonValueRef &institution : qAsConst(institutionsJsonArray))
            _institutionsModel->addFinancialInstitution(FinancialInstitution::fromJson(institution.toObject()));
    }

    // Accounts
    if (json.contains("accounts") && json["accounts"].isArray()) {
        QJsonArray accountsJsonArray = json["accounts"].toArray();
        for (const QJsonValueRef &account : qAsConst(accountsJsonArray))
            _accountModel->addAccount(Account::fromJson(account.toObject()));
    }
    // link model objects to each account
    for (Account *account : _accountModel->accounts()) {
        // link currency
        QUuid currencyUid = account->getCurrency()->getUid();
        account->setCurrency(_currencyModel->getCurrency(currencyUid));
        // link financial institution
        QUuid institutionUid = account->getInstitution()->getUid();
        account->setInstitution(_institutionsModel->getFinancialInstitution(institutionUid));
        // link owners
        QList<QUuid> ownersIds = account->getOwnersUid();
        account->getOwners().clear();
        for (QUuid ownerUid : ownersIds)
            account->addOwner(_ownerModel->getOwner(ownerUid));
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
}

TransactionModel *Model::getTransactionModel(Account *selectedAccount) {
    return new TransactionModel(selectedAccount);
}
