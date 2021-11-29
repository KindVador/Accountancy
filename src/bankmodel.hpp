#ifndef ACCOUNTANCY_BANKMODEL_HPP
#define ACCOUNTANCY_BANKMODEL_HPP

#include "core/bank.hpp"

#include <QAbstractItemModel>

class BankModel : public QAbstractListModel
{
    Q_OBJECT
public:
    BankModel() = default;
    ~BankModel() override = default;

    // QAbstractListModel interface
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    int addBank(Bank *bank);
    int addBank(QString name);

private:
    QList<Bank *> _banks;
};


#endif //ACCOUNTANCY_BANKMODEL_HPP
