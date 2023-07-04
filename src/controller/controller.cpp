#include "controller.hpp"
#include "utils/instrumentor.hpp"

#include <QApplication>
#include <QJsonDocument>
#include <QMessageBox>

constexpr const int ObjectRole = Qt::UserRole + 1;

Controller* Controller::_singleton = nullptr;
QSettings Controller::_settings = QSettings(QCoreApplication::applicationName());

Controller::Controller() : _model(Model::instance()), _mainWindow(new MainWindow)
{
    loadSettings();

    // init model
    _model->registerModel(new OwnerModel("OwnerModel"));
    _model->registerModel(new CurrencyModel("CurrencyModel"));
    _model->registerModel(new AccountModel("AccountModel"));
    _model->registerModel(new FinancialInstitutionModel("FinancialInstitutionModel"));
    _model->registerModel(new ImportConfigModel("ImportConfigModel"));
    _model->registerModel(new CategoryModel("CategoryModel"));
    _model->setAccountFilter(_model->getModel<AccountModel>("AccountModel"));

    // connect with MainWindow
    if (_mainWindow != nullptr) {
        _mainWindow->setModel(_model);
        connect(_mainWindow, &MainWindow::selectedOwnerChanged, this, &Controller::onSelectedOwner);
        connect(_mainWindow, &MainWindow::selectedAccountChanged, this, &Controller::onSelectedAccount);
    }

    // Populate some data in model for Dev only
    addCurrency("Euro", "€");
    addFinancialInstitution("Caisse d'Epargne");
}

Controller::~Controller() = default;

void Controller::addOwner(Owner* owner)
{
    if (_model == nullptr)
        return;

    if (auto model = _model->getModel<OwnerModel>("OwnerModel"); model != nullptr)
        model->addOwner(owner);

    qDebug() << " Add Owner: " << owner->getName();
}

void Controller::showMainWindow()
{
    if (_mainWindow == nullptr)
        return;

    _mainWindow->show();
}

Owner* Controller::addOwner(const QString& name, double warningBalance, const QString& comment, bool isHidden)
{
    if (_model == nullptr)
        return nullptr;

    // model update
    if (auto model = _model->getModel<OwnerModel>("OwnerModel"); model != nullptr) {
        qDebug() << " Add Owner: " << name;
        return model->addOwner(name, warningBalance, comment, isHidden);
    }


    return nullptr;
}

Account* Controller::addAccount(const FinancialInstitution* institution, AccountType type, const Currency* currency,
                                const QList<const Owner*>& owners, float initialBalance, float warningBalance,
                                const QString& accountNumber, const QString& comment, bool isIncludedInTotal,
                                bool isHidden)
{
    if (_model == nullptr)
        return nullptr;

    // model update
    if (auto model = _model->getModel<AccountModel>("AccountModel"); model != nullptr) {
        return model->addAccount(institution, type, currency, owners, initialBalance, warningBalance,
                                 accountNumber, comment, isIncludedInTotal, isHidden);
    }

    return nullptr;
}

void Controller::onSelectedOwner(const QModelIndex& index)
{
    if (!index.isValid() || _model == nullptr)
        return;

    if (auto ownerModel = _model->getModel<OwnerModel>("OwnerModel"); ownerModel != nullptr) {
        auto ownerName = ownerModel->data(index, Qt::DisplayRole).value<QString>();
        QUuid ownerUid = ownerModel->data(index, ObjectRole).value<Owner*>()->getUid();
        qDebug() << "Controller::onSelectedOwner" << index.isValid() << ownerName << ownerUid;
        // Apply filtering on Account's model
        _model->setOwnerFilter(ownerUid);
        _model->getAccountFilter()->invalidate();
    }
}

void Controller::onSelectedAccount(const QModelIndex& index)
{
    if (auto model = _model->getModel<AccountModel>("AccountModel"); model != nullptr) {
        auto accountName = model->data(index, Qt::DisplayRole).value<QString>();
        qDebug() << "Controller::onSelectedAccount" << index.isValid() << accountName;
    }
}

void Controller::clearOwnerSelection()
{
    _model->setOwnerFilter(QUuid());
    _model->getAccountFilter()->invalidate();
}

Controller* Controller::instance()
{
    if (_singleton == nullptr)
        _singleton = new Controller();

    return _singleton;
}

void Controller::addTransactionToAccount(Transaction* transaction, Account* account) const
{
    if (transaction == nullptr || account == nullptr)
        return;

    account->addTransaction(transaction);
}

void Controller::addFinancialInstitution(FinancialInstitution* institution)
{
    if (_model == nullptr)
        return;

    if (auto model = _model->getModel<FinancialInstitutionModel>("FinancialInstitutionModel"); model != nullptr)
        model->addFinancialInstitution(institution);
}

FinancialInstitution* Controller::addFinancialInstitution(const QString& name)
{
    if (auto model = _model->getModel<FinancialInstitutionModel>("FinancialInstitutionModel"); model != nullptr)
        return model->addFinancialInstitution(name);

    return nullptr;
}

const QString& Controller::getCurrentFilePath() const
{
    return _currentFilePath;
}

void Controller::setCurrentFilePath(const QString& currentFilePath)
{
    _currentFilePath = currentFilePath;
    // add file in the window title
    _mainWindow->setWindowTitle(QString("Accountancy - %1").arg(_currentFilePath));
}

bool Controller::saveToFile(const QString& filePath)
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

bool Controller::loadFile(const QString& filePath)
{
    qDebug() << "loading file: " << filePath;
    Instrumentor::Get().BeginSession("Session1");
    {
        // Timer
        InstrumentationTimer timer("loadFile");

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
    }

    _mainWindow->updateEditionInterface(true);
    Instrumentor::Get().EndSession();
    return true;
}

bool Controller::createNewFile(const QString& filePath)
{
    // check that mode is not null
    if (_model == nullptr)
        return false;

    // check if a model has unsaved modifications
    if (_model->isDirty()) {
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

void Controller::addCurrency(Currency* currency)
{
    if (_model == nullptr)
        return;

    if (auto model = _model->getModel<CurrencyModel>("CurrencyModel"); model != nullptr)
        model->addCurrency(currency);
}

Currency* Controller::addCurrency(const QString& name, const QString& symbol)
{
    if (_model == nullptr)
        return nullptr;

    if (auto model = _model->getModel<CurrencyModel>("CurrencyModel"); model != nullptr)
        return model->addCurrency(name, symbol);

    return nullptr;
}

void Controller::removeCurrency(const QUuid& uid)
{
    if (_model == nullptr || uid.isNull())
        return;

    if (auto model = _model->getModel<CurrencyModel>("CurrencyModel"); model != nullptr)
        model->removeCurrency(uid);
}

void Controller::removeInstitution(const QUuid& uid)
{
    if (auto model = _model->getModel<FinancialInstitutionModel>("FinancialInstitutionModel"); model != nullptr)
        model->removeFinancialInstitution(uid);
}

void Controller::addCategory(Category* category, Category* parent)
{
    if (_model == nullptr)
        return;

    if (auto model = _model->getModel<CategoryModel>("CategoryModel"); model != nullptr)
        model->addCategory(category, parent);
}

Category* Controller::addCategory(const QString& name, Category* parent)
{
    if (_model == nullptr)
        return nullptr;

    if (auto model = _model->getModel<CategoryModel>("CategoryModel"); model != nullptr)
        return model->addCategory(name, parent);

    return nullptr;
}

void Controller::removeCategory(const QUuid& uid)
{
    if (_model == nullptr || uid.isNull())
        return;

    if (auto model = _model->getModel<CategoryModel>("CategoryModel"); model != nullptr)
        model->removeCategory(uid);
}

void Controller::loadSettings()
{
    QString previous_file = _settings.value("last_loaded_file").toString();
    if (!previous_file.isEmpty())
        _currentFilePath = previous_file;
}

void Controller::saveSettings() const
{
    if (!_settings.isWritable()) {
        qDebug() << "ERROR: the file [" << _settings.fileName() << "] is not writable.";
        return;
    }

    if (!_currentFilePath.isEmpty())
        _settings.setValue("last_loaded_file", _currentFilePath);
}

bool Controller::loadFromSettings()
{
    QString previous_file = _settings.value("last_loaded_file").toString();
    if (!previous_file.isEmpty())
        return loadFile(previous_file);
    else
        return false;
}

QList<Transaction*> Controller::readTransactionsFromFile(QFile& dataFile, const ImportConfig& config, const Currency* currency, const CategoryModel* category_model) const
{
    if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    QLocale locale = QLocale::system();
    bool replaceDecimalCharacter = config.getDecimalChar() != locale.decimalPoint();

    QList<Transaction*> transactions;
    QVector<QString> rawLines;
    QTextStream inStream(&dataFile);
    // Reads the data up to the end of file
    while (!inStream.atEnd())
        rawLines.append(inStream.readLine());

    int firstLineToImport = config.getNbLinesToSkipStart() == 0 ? 0 : config.getNbLinesToSkipStart();
    int nbLinesToImport = (int) rawLines.count() - (config.getNbLinesToSkipStart() + config.getNbLinesToSkipEnd()) + 1;
    rawLines = rawLines.mid(firstLineToImport, nbLinesToImport);

    for (const QString& rawLine: qAsConst(rawLines)) {
        QStringList fields = rawLine.split(config.getSeparatorChar());

        if (fields.count() < config.nbFields())
            continue;

        auto* transaction = new Transaction();
        // Name
        if (int name_position = config.getColumnPosition("Name"); name_position >= 0)
            transaction->setName(fields[name_position]);
        // Comment
        if (int comment_position = config.getColumnPosition("Comment"); comment_position >= 0)
            transaction->setComment(fields[comment_position]);

        // Data and Time
        if (int date_position = config.getColumnPosition("Date"); date_position >= 0) {
            QDate date = locale.toDate(fields[date_position], config.getDateFormat());
            // fix year date as 20 is interpreted as 1920 instead of 2020
            if (date.year() + 100 <= QDate::currentDate().year())
                date = date.addYears(100);
            QTime time;
            int time_position = config.getColumnPosition("Time");
            if (config.hasTime() && time_position >= 0) {
                QString timeString = fields[config.getColumnPosition("Time")];
                const QString& timeFormat = config.getTimeFormat();
                if ((timeString.length() > timeFormat.length()) && timeString.length() > 28)
                    timeString = timeString.mid(17, 12);
                time = QTime::fromString(timeString, timeFormat);
            }
            transaction->setDateTime(QDateTime(date, time));
        }

        // Amount
        QString amountValue;
        if (!fields[config.getColumnPosition("DebitAmount")].isEmpty())
            amountValue = fields[config.getColumnPosition("DebitAmount")];
        else
            amountValue = fields[config.getColumnPosition("CreditAmount")];

        if (replaceDecimalCharacter)
            amountValue.replace(config.getDecimalChar(), locale.decimalPoint());

        if (currency != nullptr && amountValue.contains(currency->getSymbol()))
            amountValue.replace(currency->getSymbol(), "", Qt::CaseInsensitive);

        transaction->setAmount(locale.toDouble(amountValue));

        // Category
        if (int category_position = config.getColumnPosition("Category"); category_model != nullptr && category_position >= 0) {
            transaction->setCategory(category_model->category(fields[category_position]));
        }

        transactions.append(transaction);
    }
    dataFile.close();

    return transactions;
}