#ifndef ACCOUNTANCY_ACCOUNTMODEL_HPP
#define ACCOUNTANCY_ACCOUNTMODEL_HPP

#include "abstractmodel.hpp"
#include "core/account.hpp"

#include <QAbstractItemModel>

class AccountModel : public QAbstractListModel, public AbstractModel
{
    Q_OBJECT
public:
    explicit AccountModel(QString name);
    ~AccountModel() override = default;

    // AbstractModel
    [[nodiscard]] bool isDirty() const override;
    void reset() override;
    void write(QJsonObject& json) const override;
    void read(const QJsonObject& json) override;

    // QAbstractListModel interface
    [[nodiscard]] int rowCount(const QModelIndex& parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;

    void addAccount(Account* account);
    Account* addAccount(const FinancialInstitution* institution, AccountType type, const Currency* currency,
                        const QList<const Owner*>& owners, float initialBalance, float warningBalance,
                        const QString& accountNumber, const QString& comment, bool isIncludedInTotal, bool isHidden);
    void removeAccount(const QModelIndex& index);

    [[nodiscard]] const QList<Account*>& accounts() const;

private:
    QList<Account*> _accounts;
};

#endif//ACCOUNTANCY_ACCOUNTMODEL_HPP
