#include "transactionmodel.hpp"

#include <QColor>

QList<QString> TransactionModel::headerLabels = {"Date", "Name", "Amount", "Status", "Comment"};

TransactionModel::TransactionModel(Account *account): _account(account)
{
}

int TransactionModel::rowCount(const QModelIndex &parent) const
{
    if (_account == nullptr)
        return 0;

    return _account->count();
}

QVariant TransactionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || _account == nullptr)
        return {};

    Transaction *t = _account->transactionAt(index.row());
    const Currency *currency = _account->getCurrency();
    if (role == Qt::DisplayRole) {
        if (t == nullptr)
            return {};

        switch (index.column()) {
            case 0:
                return t->getDate().toString("dd/MM/yyyy");
            case 1:
                return t->getName();
            case 2:
                return QString("%1%2").arg(QString::number(t->getAmount()), currency->getSymbol());
            case 3:
                return TRANSACTION_STATUS_2_STRING[t->getStatus()];
            case 4:
                return t->getComment();
            default:
                return {};
        }
    } else if (role == Qt::ForegroundRole && index.column() == 2) {
        if (t->getAmount() < 0)
            return QColorConstants::Red;
        else
            return QColorConstants::Green;
    }
    return {};
}

int TransactionModel::columnCount(const QModelIndex &parent) const
{
    return (int)headerLabels.count();
}

QVariant TransactionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Orientation::Horizontal && role == Qt::DisplayRole && section < headerLabels.count())
        return headerLabels.at(section);
    return {};
}

void TransactionModel::reset()
{
    beginResetModel();
    _account = nullptr;
    endResetModel();
}
