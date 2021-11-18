#ifndef ACCOUNTANCY_ACCOUNTMODEL_HPP
#define ACCOUNTANCY_ACCOUNTMODEL_HPP

#include "core/account.hpp"

#include <QAbstractItemModel>

class AccountModel : public QAbstractListModel
{

    Q_OBJECT

public:
    AccountModel();
    ~AccountModel() override = default;

    // QAbstractListModel interface
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    int addAccount(Account *account);
    int addAccount(AccountType type, Currency *currency, const Owner *owner, float initialBalance, float warningBalance,
                   const QString &accountNumber, const QString &comment, bool isIncludedInTotal, bool isHidden);
    void removeAccount(Account *account);
    void removeAccount(int id);
    Account* getAccount(const QString &name);

private:
    QList<Account*> _accounts;
};

#endif //ACCOUNTANCY_ACCOUNTMODEL_HPP
