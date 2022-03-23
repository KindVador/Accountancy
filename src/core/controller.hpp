#ifndef ACCOUNTANCY_CONTROLLER_HPP
#define ACCOUNTANCY_CONTROLLER_HPP

#include "financialinstitution.hpp"
#include "owner.hpp"
#include "account.hpp"
#include "transaction.hpp"
#include "model.hpp"
#include "../ui/mainwindow.hpp"

#include <QObject>

class Controller : public QObject
{
    Q_OBJECT

public:
    // Singleton
    static Controller *getInstance();
    Controller(Controller &other) = delete;

    ~Controller() override;

    void showMainWindow();

    // CURRENCIES
    void addCurrency(Currency *currency);
    Currency *addCurrency(const QString &name, const QString &symbol);

    // FINANCIAL INSTITUTIONS
    void addFinancialInstitution(FinancialInstitution *institution);
    FinancialInstitution *addFinancialInstitution(const QString &name);

    // OWNERS
    void addOwner(Owner *owner);
    Owner *addOwner(const QString &name, float warningBalance, const QString &comment, bool isHidden);

    // ACCOUNTS
    Account *addAccount(const FinancialInstitution *institution, AccountType type, Currency *currency,
                        const Owner *owner, float initialBalance, float warningBalance,
                        const QString &accountNumber, const QString &comment, bool isIncludedInTotal, bool isHidden);

    // TRANSACTIONS
    void addTransactionToAccount(Transaction *transaction, Account *account);

    // CREATE, LOAD or SAVE MODEL TO FILE
    bool createNewFile(const QString &filePath);
    bool loadFile(const QString &filePath);
    bool saveToFile(const QString &filePath);

    // CURRENT FILE
    [[nodiscard]] const QString &getCurrentFilePath() const;
    void setCurrentFilePath(const QString &currentFilePath);

public slots:
    void clearOwnerSelection();

private slots:
    void onSelectedOwner(const QModelIndex &index);
    void onSelectedAccount(const QModelIndex &index);

private:
    static Controller *_singleton;
    Model *_model = nullptr;
    MainWindow *_mainWindow = nullptr;
    QString _currentFilePath;

    // Singleton
    explicit Controller();
};

#endif //ACCOUNTANCY_CONTROLLER_HPP
