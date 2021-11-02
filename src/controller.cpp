#include "controller.hpp"

Controller::Controller(): _model(new Model), _mainWindow(new MainWindow)
{

    // model init for debug
    auto *euro = new Currency();
    euro->setName("Euro");
    euro->setSymbol("€");
    auto *florian = new Owner("Florian", euro, 0, "", false);
    addOwner(florian);
    addAccount(AccountType::Checking, euro, florian, 100, 200, "xxyy", "my comment", true, false);

    // connect mainwindow to model
    if (_mainWindow != nullptr)
        _mainWindow->setModel(_model);
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

    // view update
    _mainWindow->onOwnerModelUpdate();
}

void Controller::addAccount(Account *account) {

}

void Controller::addAccount(AccountType type, Currency *currency, const Owner *owner, float initialBalance,
                            float warningBalance, const QString &accountNumber, const QString &comment,
                            bool isIncludedInTotal, bool isHidden) {

}
