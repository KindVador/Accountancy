#ifndef ACCOUNTANCY_ACCOUNTMODEL_HPP
#define ACCOUNTANCY_ACCOUNTMODEL_HPP

#include "account.hpp"

#include <QAbstractItemModel>

class AccountModel : public QAbstractListModel
{
    Q_OBJECT
public:
    AccountModel() = default;
    ~AccountModel() override = default;

    // QAbstractListModel interface
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    void addAccount(Account *account);
    Account *addAccount(const FinancialInstitution *institution, AccountType type, Currency *currency,
                        const QList<const Owner*> &owners, float initialBalance, float warningBalance,
                        const QString &accountNumber, const QString &comment, bool isIncludedInTotal, bool isHidden);
    void removeAccount(const QModelIndex &index);

    [[nodiscard]] const QList<Account *>& accounts() const;

    void reset();

private:
    QList<Account *> _accounts;
};

#endif //ACCOUNTANCY_ACCOUNTMODEL_HPP
