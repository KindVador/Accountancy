#ifndef ACCOUNTANCY_CONTROLLER_HPP
#define ACCOUNTANCY_CONTROLLER_HPP

#include "src/core/owner.hpp"
#include "src/core/account.hpp"
#include "model.hpp"
#include "ui/mainwindow.hpp"

#include <QObject>

class Controller : public QObject
{
    Q_OBJECT

public:
    explicit Controller();
    ~Controller() override;

    void showMainWindow();

    void addOwner(Owner *owner);
    void addOwner(const QString &name, const Currency *currency, float warningBalance, const QString &comment, bool isHidden);

    void addAccount(AccountType type, Currency *currency, const Owner *owner, float initialBalance, float warningBalance,
                    const QString &accountNumber, const QString &comment, bool isIncludedInTotal, bool isHidden);

private slots:
    void onSelectedOwner(const QModelIndex &index);
    void onSelectedAccount(const QModelIndex &index);

private:
    Model *_model = nullptr;
    MainWindow *_mainWindow = nullptr;
};

#endif //ACCOUNTANCY_CONTROLLER_HPP
