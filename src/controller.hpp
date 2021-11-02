#ifndef ACCOUNTANCY_CONTROLLER_HPP
#define ACCOUNTANCY_CONTROLLER_HPP

#include "src/core/owner.hpp"
#include "src/core/account.hpp"
#include "model.hpp"
#include "ui/mainwindow.hpp"

class Controller {

public:
    explicit Controller();
    ~Controller();

    void showMainWindow();

    void addOwner(Owner *owner);
    void addOwner(const QString &name, const Currency *currency, float warningBalance, const QString &comment, bool isHidden);

    void addAccount(Account *account);
    void addAccount(AccountType type, Currency *currency, const Owner *owner, float initialBalance, float warningBalance,
                    const QString &accountNumber, const QString &comment, bool isIncludedInTotal, bool isHidden);

private:
    Model *_model = nullptr;
    MainWindow *_mainWindow = nullptr;
};

#endif //ACCOUNTANCY_CONTROLLER_HPP
