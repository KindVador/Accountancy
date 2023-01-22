#ifndef ACCOUNTANCY_CONTROLLER_HPP
#define ACCOUNTANCY_CONTROLLER_HPP

#include "../ui/mainwindow.hpp"
#include "account.hpp"
#include "financialinstitution.hpp"
#include "model.hpp"
#include "owner.hpp"
#include "transaction.hpp"

#include <QObject>

class Controller : public QObject
{
    Q_OBJECT

public:
    // Singleton
    static Controller* instance();
    Controller(Controller& other) = delete;

    ~Controller() override;

    void showMainWindow();

    // CATEGORIES
    void addCategory(Category* category);
    Category* addCategory(const QString& name);
    void removeCategory(const QUuid& uid);

    // CURRENCIES
    void addCurrency(Currency* currency);
    Currency* addCurrency(const QString& name, const QString& symbol);
    void removeCurrency(const QUuid& uid);

    // FINANCIAL INSTITUTIONS
    void addFinancialInstitution(FinancialInstitution* institution);
    FinancialInstitution* addFinancialInstitution(const QString& name);
    void removeInstitution(const QUuid& uid);

    // OWNERS
    void addOwner(Owner* owner);
    Owner* addOwner(const QString& name, double warningBalance, const QString& comment, bool isHidden);

    // ACCOUNTS
    Account* addAccount(const FinancialInstitution* institution, AccountType type, const Currency* currency,
                        const QList<const Owner*>& owners, float initialBalance, float warningBalance,
                        const QString& accountNumber, const QString& comment, bool isIncludedInTotal, bool isHidden);

    // TRANSACTIONS
    void addTransactionToAccount(Transaction* transaction, Account* account) const;

    // CREATE, LOAD or SAVE MODEL TO FILE
    bool createNewFile(const QString& filePath);
    bool loadFile(const QString& filePath);
    bool saveToFile(const QString& filePath);

    // CURRENT FILE
    [[nodiscard]] const QString& getCurrentFilePath() const;
    void setCurrentFilePath(const QString& currentFilePath);

public slots:
    void clearOwnerSelection();

private slots:
    void onSelectedOwner(const QModelIndex& index);
    void onSelectedAccount(const QModelIndex& index);

private:
    static Controller* _singleton;
    Model* _model = nullptr;
    MainWindow* _mainWindow = nullptr;
    QString _currentFilePath;

    // Singleton
    explicit Controller();
};

#endif//ACCOUNTANCY_CONTROLLER_HPP
