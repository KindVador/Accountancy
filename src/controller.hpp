#ifndef ACCOUNTANCY_CONTROLLER_HPP
#define ACCOUNTANCY_CONTROLLER_HPP

#include "core/financialinstitution.hpp"
#include "core/owner.hpp"
#include "core/account.hpp"
#include "core/transaction.hpp"
#include "model.hpp"
#include "ui/mainwindow.hpp"

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

    // FINANCIAL INSTITUTIONS
    void addFinancialInstitution(FinancialInstitution *institution);
    void addFinancialInstitution(QString name);

    // OWNERS
    void addOwner(Owner *owner);
    void addOwner(const QString &name, const Currency *currency, float warningBalance, const QString &comment, bool isHidden);

    // ACCOUNTS
    void addAccount(AccountType type, Currency *currency, const Owner *owner, float initialBalance, float warningBalance,
                    const QString &accountNumber, const QString &comment, bool isIncludedInTotal, bool isHidden);

    // TRANSACTIONS
    void addTransactionToAccount(Transaction *transaction, Account *account);

public slots:
    void clearOwnerSelection();

private slots:
    void onSelectedOwner(const QModelIndex &index);
    void onSelectedAccount(const QModelIndex &index);

private:
    static Controller *_singleton;
    Model *_model = nullptr;
    MainWindow *_mainWindow = nullptr;

    // Singleton
    Controller();
};

#endif //ACCOUNTANCY_CONTROLLER_HPP
