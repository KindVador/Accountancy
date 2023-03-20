#include "transactionmodel.hpp"
#include "category.hpp"

#include <QColor>
#include <utility>

QList<QString> TransactionModel::headerLabels = {"Date", "Name", "Amount", "Balance", "Status", "Category", "Comment"};

constexpr const int ObjectRole = Qt::UserRole + 1;
constexpr const int UIDRole = Qt::UserRole + 2;

TransactionModel::TransactionModel(QString name) : AbstractModel(std::move(name))
{
}

TransactionModel::TransactionModel(Account* account) : AbstractModel(account != nullptr ? account->getUid().toString() : ""), _account(account)
{
}

int TransactionModel::rowCount(const QModelIndex& parent) const
{
    if (_account == nullptr)
        return 0;

    return _account->count();
}

QVariant TransactionModel::data(const QModelIndex& index, int role) const
{
    qDebug() << "TransactionModel::data() " << index.row() << " " << role;
    if (!index.isValid() || _account == nullptr)
        return {};

    const Transaction* t = _account->transactionAt(index.row());
    if (t == nullptr)
        return {};

    const Currency* currency = _account->getCurrency();
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
                return t->getDateTime().toString("dd/MM/yyyy");
            case 1:
                return t->getName();
            case 2:
                return QString("%1%2").arg(QString::number(t->getAmount()), currency->getSymbol());
            case 3:
                return QString("%1%2").arg(QString::number(t->getCurrentBalance()), currency->getSymbol());
            case 4:
                return TRANSACTION_STATUS_2_STRING[t->getStatus()];
            case 5: {
                const Category* category = t->getCategory();
                if (category != nullptr)
                    return category->getName();
                else
                    return {};
            }
            case 6:
                return t->getComment();
            default:
                return {};
        }
    } else if (role == Qt::ForegroundRole && (index.column() == 2 || index.column() == 3)) {
        double cellValue = index.column() == 2 ? t->getAmount() : t->getCurrentBalance();
        if (cellValue < 0)
            return QColorConstants::DarkRed;
        else
            return QColorConstants::DarkGreen;
    } else if (role == Qt::UserRole) {
        switch (index.column()) {
            case 0:
                return t->getDateTime();
            case 1:
                return t->getName();
            case 2:
                return t->getAmount();
            case 3:
                return t->getCurrentBalance();
            case 4:
                return TRANSACTION_STATUS_2_STRING[t->getStatus()];
            case 5:
                return t->getComment();
            default:
                return {};
        }
    } else if (role == UIDRole) {
        return t->getUid();
    }
    return {};
}

int TransactionModel::columnCount(const QModelIndex& parent) const
{
    return (int) headerLabels.count();
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

bool TransactionModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (_account == nullptr)
        return false;

    beginRemoveRows(parent, row, row + count - 1);
    for (int i = row; i < row + count; ++i) {
        Transaction* transaction = _account->transactionAt(i);
        if (transaction == nullptr)
            continue;

        _account->removeTransaction(transaction);
    }
    endRemoveRows();
    return true;
}

bool TransactionModel::removeTransaction(QUuid transactionUid)
{
    bool res = false;
    beginResetModel();
    res = _account->removeTransaction(transactionUid);
    endResetModel();
    return res;
}

Account* TransactionModel::getAccount() const
{
    return _account;
}

void TransactionModel::setAccount(Account* account)
{
    beginResetModel();
    _account = account;
    endResetModel();
}

bool TransactionModel::isDirty() const
{
    // TODO implement TransactionModel::isDirty()
    return false;
}

void TransactionModel::write(QJsonObject& json) const
{
}

void TransactionModel::read(const QJsonObject& json)
{
}
