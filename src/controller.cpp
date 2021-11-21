#include "controller.hpp"

constexpr const int ObjectRole = Qt::UserRole + 1;

Controller::Controller(): _model(new Model), _mainWindow(new MainWindow)
{

    // model init for debug
    auto *euro = new Currency();
    euro->setName("Euro");
    euro->setSymbol("€");
    auto *florian = new Owner("Florian", euro, 0, "", false);
    auto *toto = new Owner("Toto", euro, 0, "", false);
    addOwner(florian);
    addOwner(toto);
    addAccount(AccountType::Checking, euro, florian, 100, 200, "ACCOUNT_FLORIAN_1", "my comment", true, false);
    addAccount(AccountType::Checking, euro, florian, 1000, 500, "ACCOUNT_FLORIAN_2", "my comment", true, false);
    addAccount(AccountType::Checking, euro, toto, 750, 50, "ACCOUNT_TOTO", "my comment", true, false);

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

void Controller::addOwner(const QString &name, const Currency *currency, float warningBalance, const QString &comment, bool isHidden)
{
    if (_model == nullptr)
        return;

    // model update
    int ownerId = _model->getOwnerModel()->addOwner(name, currency, warningBalance, comment, isHidden);
    qDebug() << "addOwner" << name << ownerId;

    // view update
    _mainWindow->onOwnerModelUpdate();
}

void Controller::addAccount(AccountType type, Currency *currency, const Owner *owner, float initialBalance, float warningBalance, const QString &accountNumber, const QString &comment, bool isIncludedInTotal, bool isHidden)
{
    if (_model == nullptr)
        return;

    // model update
    int accountId = _model->getAccountModel()->addAccount(type, currency, owner, initialBalance, warningBalance,
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
