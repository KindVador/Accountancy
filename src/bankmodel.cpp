#include "bankmodel.hpp"

constexpr const int ObjectRole = Qt::UserRole + 1;

int BankModel::rowCount(const QModelIndex &parent) const
{
    return (int) _banks.count();
}

QVariant BankModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= _banks.count() || index.row() < 0)
        return {};

    QVariant v;
    switch (role) {
        case Qt::DisplayRole:
            v.setValue(_banks.at(index.row())->getName());
            break;
        case Qt::UserRole:
            v.setValue(_banks.at(index.row())->getId());
            break;
        case ObjectRole:
            v.setValue(_banks.at(index.row()));
            break;
        default:
            break;
    }
    return v;
}

int BankModel::addBank(Bank *bank)
{
    if (bank == nullptr)
        return -1;

    int nextId = -1;
    if (_banks.isEmpty())
        nextId = 0;
    else
        nextId = _banks.last()->getId() + 1;

    bank->setId(nextId);
    _banks.append(bank);
    return nextId;
}

int BankModel::addBank(QString name)
{
    auto *newBank = new Bank(name);
    return addBank(newBank);
}
