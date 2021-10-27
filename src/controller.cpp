#include "controller.hpp"

Controller::Controller(): _model(new Model), _mainWindow(new MainWindow)
{

    // model init for debug
    Currency euro = Currency();
    euro.setName("Euro");
    euro.setSymbol("€");
    addOwner("Florian", euro, 0, "", false);

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

void Controller::addOwner(const QString &name, const Currency &currency, float warningBalance, const QString &comment, bool isHidden)
{
    if (_model == nullptr)
        return;

    // model update
    int ownerId = _model->getOwnerModel()->addOwner(name, currency, warningBalance, comment, isHidden);

    // view update
    _mainWindow->onOwnerModelUpdate();
}
