#ifndef ACCOUNTANCY_TRANSACTIONMODEL_HPP
#define ACCOUNTANCY_TRANSACTIONMODEL_HPP

#include <QAbstractTableModel>
#include "core/account.hpp"

class TransactionModel : public QAbstractTableModel
{
public:
    TransactionModel() = default;
    explicit TransactionModel(Account *account);
    ~TransactionModel() override = default;

    // QAbstractItemModel
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    // QAbstractTableModel
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    void reset();

private:
    Account *_account = nullptr;
    static QList<QString> headerLabels;
};


#endif //ACCOUNTANCY_TRANSACTIONMODEL_HPP
