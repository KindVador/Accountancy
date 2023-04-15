#ifndef ACCOUNTANCY_TRANSACTIONMODEL_HPP
#define ACCOUNTANCY_TRANSACTIONMODEL_HPP

#include <QAbstractTableModel>

#include "abstractmodel.hpp"
#include "core/account.hpp"
#include "core/transaction.hpp"

class TransactionModel : public QAbstractTableModel, public AbstractModel
{
public:
    explicit TransactionModel(QString name);
    explicit TransactionModel(Account* account);
    ~TransactionModel() override = default;

    // AbstractModel
    [[nodiscard]] bool isDirty() const override;
    void reset() override;
    void write(QJsonObject& json) const override;
    void read(const QJsonObject& json) override;

    // QAbstractItemModel
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool removeRows(int row, int count, const QModelIndex& parent) override;

    // QAbstractTableModel
    [[nodiscard]] int rowCount(const QModelIndex& parent) const override;
    [[nodiscard]] int columnCount(const QModelIndex& parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;

    [[nodiscard]] Account* getAccount() const;
    void setAccount(Account* account);

    bool removeTransaction(QUuid transactionUid);

private:
    Account* _account = nullptr;
    static QList<QString> headerLabels;
};

#endif//ACCOUNTANCY_TRANSACTIONMODEL_HPP
