#include <QJsonArray>
#include "model.hpp"

constexpr const int ObjectRole = Qt::UserRole + 1;

Model *Model::_singleton = nullptr;

Model *Model::getInstance()
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

void Model::setOwnerFilter(int ownerId)
{
    if (_accountFilteredModel == nullptr)
        return;

    qWarning() << "Model::setOwnerFilter" << ownerId;
    _accountFilteredModel->setActiveOwnerId(ownerId);
}

void Model::setOwnerFilter(const QString &ownerName)
{
    if (_accountFilteredModel == nullptr)
        return;

    qWarning() << "Model::setOwnerFilter" << ownerName;
    Owner *owner = _ownerModel->getOwner(ownerName);
    _accountFilteredModel->setActiveOwnerId(owner->getId());
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
    // Currencies
    QJsonArray currencies;
    for (int i = 0; i < _currencyModel->rowCount(QModelIndex()); ++i) {
        auto *currency = _currencyModel->data(_currencyModel->index(i, 0), ObjectRole).value<Currency *>();
        QJsonObject currencyJson;
        currency->write(currencyJson);
        currencies.append(currencyJson);
    }
    json["currencies"] = currencies;

    // Owners
    QJsonArray owners;
    for (int i = 0; i < _ownerModel->rowCount(QModelIndex()); ++i) {
        auto *owner = _ownerModel->data(_ownerModel->index(i, 0), ObjectRole).value<Owner *>();
        QJsonObject ownerJson;
        owner->write(ownerJson);
        owners.append(ownerJson);
    }
    json["owners"] = owners;

    // Institutions
    QJsonArray institutions;
    for (int i = 0; i < _institutionsModel->rowCount(QModelIndex()); ++i) {
        auto *institution = _institutionsModel->data(_institutionsModel->index(i, 0), ObjectRole).value<FinancialInstitution *>();
        QJsonObject institutionJson;
        institution->write(institutionJson);
        institutions.append(institutionJson);
    }
    json["institutions"] = institutions;

    // Accounts
    QJsonArray accounts;
    for (int i = 0; i < _accountModel->rowCount(QModelIndex()); ++i) {
        auto *account = _accountModel->data(_accountModel->index(i, 0), ObjectRole).value<Account *>();
        QJsonObject accountJson;
        account->write(accountJson);
        accounts.append(accountJson);
    }
    json["accounts"] = accounts;
}

void Model::read(QJsonObject &json)
{

}
