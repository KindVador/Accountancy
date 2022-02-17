#include "controller.hpp"

#include <QJsonDocument>
#include <QMessageBox>

constexpr const int ObjectRole = Qt::UserRole + 1;

Controller *Controller::_singleton = nullptr;

Controller::Controller(): _model(Model::getInstance()), _mainWindow(new MainWindow)
{
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

Owner *Controller::addOwner(const QString &name, float warningBalance, const QString &comment, bool isHidden)
{
    if (_model == nullptr)
        return nullptr;

    // model update
    auto ownersModel = _model->getOwnerModel();
    if (ownersModel != nullptr)
        return ownersModel->addOwner(name, warningBalance, comment, isHidden);

    return nullptr;
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

const QString &Controller::getCurrentFilePath() const
{
    return _currentFilePath;
}

void Controller::setCurrentFilePath(const QString &currentFilePath)
{
    _currentFilePath = currentFilePath;
    // add file in the window title
    _mainWindow->setWindowTitle(QString("Accountancy - %1").arg(_currentFilePath));
}

bool Controller::saveToFile(const QString &filePath)
{
    setCurrentFilePath(filePath);
    QFile saveFile(filePath);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QJsonObject modelJsonObject;
    _model->write(modelJsonObject);
    // JSON format
    saveFile.write(QJsonDocument(modelJsonObject).toJson());

    // BINARY format
    //saveFile.write(QCborValue::fromJsonValue(accountancyObject).toCbor());

    return true;
}

bool Controller::loadFile(const QString &filePath)
{
    setCurrentFilePath(filePath);
    QFile openedFile(filePath);

    if (!openedFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open file.");
        return false;
    }

    QByteArray saveData = openedFile.readAll();
    QJsonDocument json = QJsonDocument::fromJson(saveData);
    _model->reset();
    _model->read(json.object());

    return true;
}

bool Controller::createNewFile(const QString &filePath)
{
    // check if a model has unsaved modifications
    if (_model != nullptr && _model->isDirty()) {
        // ask user if he wants to save modifications
        int res = QMessageBox::question(nullptr,
                                        "Save modifications",
                                        "Current opened file has unsaved changes.\n\nDo you want to save them ?");
        if (res == QMessageBox::Yes)
            saveToFile(getCurrentFilePath());
    }

    // reset model
    _model->reset();

    setCurrentFilePath(filePath);
    QFile saveFile(filePath);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't create file.");
        return false;
    }

    QJsonObject modelJsonObject;
    _model->write(modelJsonObject);
    saveFile.write(QJsonDocument(modelJsonObject).toJson());

    return true;
}
