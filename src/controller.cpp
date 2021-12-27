#include "controller.hpp"

constexpr const int ObjectRole = Qt::UserRole + 1;

Controller *Controller::_singleton = nullptr;

Controller::Controller(): _model(Model::getInstance()), _mainWindow(new MainWindow)
{
    // model init for debug
    auto *euro = new Currency();
    euro->setName("Euro");
    euro->setSymbol("€");
    const FinancialInstitution *ce = addFinancialInstitution("Caisse d'Epargne");
    auto *florian = new Owner("Florian", euro, 0, "", false);
    auto *toto = new Owner("Toto", euro, 0, "", false);
    addOwner(florian);
    addOwner(toto);
    addAccount(ce, AccountType::Checking, euro, florian, 100, 200, "ACCOUNT_FLORIAN_1", "my comment", true, false);
    addAccount(ce, AccountType::Checking, euro, florian, 1000, 500, "ACCOUNT_FLORIAN_2", "my comment", true, false);
    addAccount(ce, AccountType::Checking, euro, toto, 750, 50, "ACCOUNT_TOTO", "my comment", true, false);

    // connect with MainWindow
    if (_mainWindow != nullptr) {
        _mainWindow->setModel(_model);
        connect(_mainWindow, &MainWindow::selectedOwnerChanged, this, &Controller::onSelectedOwner);
        connect(_mainWindow, &MainWindow::selectedAccountChanged, this, &Controller::onSelectedAccount);
    }
}

Controller::~Controller() = default;

void Controller::addOwner(Owner *owner)
{
    if (_model == nullptr)
        return;

    _model->getOwnerModel()->addOwner(owner);
}

void Controller::showMainWindow()
{
    if (_mainWindow == nullptr)
        return;

    _mainWindow->show();
}

Owner *Controller::addOwner(const QString &name, const Currency *currency, float warningBalance, const QString &comment, bool isHidden)
{
    if (_model == nullptr)
        return nullptr;

    // model update
    Owner *newOwner = _model->getOwnerModel()->addOwner(name, currency, warningBalance, comment, isHidden);

    // view update
    _mainWindow->onOwnerModelUpdate();

    return newOwner;
}

Account *Controller::addAccount(const FinancialInstitution *institution, AccountType type, Currency *currency,
                                const Owner *owner, float initialBalance, float warningBalance,
                                const QString &accountNumber, const QString &comment, bool isIncludedInTotal,
                                bool isHidden)
{
    if (_model == nullptr)
        return nullptr;

    // model update
    return _model->getAccountModel()->addAccount(institution, type, currency, owner, initialBalance, warningBalance,
                                                 accountNumber, comment, isIncludedInTotal, isHidden);

}

void Controller::onSelectedOwner(const QModelIndex &index)
{
    if (!index.isValid() || _model == nullptr)
        return;

    OwnerModel *model = _model->getOwnerModel();
    auto ownerName = model->data(index, Qt::DisplayRole).value<QString>();
    int ownerId = model->data(index, ObjectRole).value<Owner*>()->getId();
    qDebug() << "Controller::onSelectedOwner" << index.isValid() << ownerName << ownerId;

    // Apply filtering on Account's model
    _model->setOwnerFilter(ownerId);
    _model->getAccountFilter()->invalidate();
}

void Controller::onSelectedAccount(const QModelIndex &index)
{
    AccountModel *model = _model->getAccountModel();
    auto accountName = model->data(index, Qt::DisplayRole).value<QString>();
    qDebug() << "Controller::onSelectedAccount" << index.isValid() << accountName;
}

void Controller::clearOwnerSelection()
{
    _model->setOwnerFilter(-1);
    _model->getAccountFilter()->invalidate();
}

Controller *Controller::getInstance()
{
    if (_singleton == nullptr)
        _singleton = new Controller();

    return _singleton;
}

void Controller::addTransactionToAccount(Transaction *transaction, Account *account)
{
    if (transaction == nullptr || account == nullptr)
        return;

    account->addTransaction(transaction);
}

void Controller::addFinancialInstitution(FinancialInstitution *institution)
{
    if (_model == nullptr)
        return;

    _model->getFinancialInstitutionModel()->addFinancialInstitution(institution);
}

FinancialInstitution *Controller::addFinancialInstitution(QString name)
{
    return _model->getFinancialInstitutionModel()->addFinancialInstitution(name);
}
