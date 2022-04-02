#include "financialinstitutionmodel.hpp"

constexpr const int ObjectRole = Qt::UserRole + 1;

int FinancialInstitutionModel::rowCount(const QModelIndex &parent) const
{
    return (int) _institutions.count();
}

QVariant FinancialInstitutionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= _institutions.count() || index.row() < 0)
        return {};

    QVariant v;
    switch (role) {
        case Qt::DisplayRole:
            v.setValue(_institutions.at(index.row())->getName());
            break;
        case Qt::UserRole:
            v.setValue(_institutions.at(index.row())->getId());
            break;
        case ObjectRole:
            v.setValue(_institutions.at(index.row()));
            break;
        default:
            break;
    }
    return v;
}

int FinancialInstitutionModel::addFinancialInstitution(FinancialInstitution *institution)
{
    if (institution == nullptr)
        return -1;

    int nextId = -1;
    if (_institutions.isEmpty())
        nextId = 0;
    else
        nextId = _institutions.last()->getId() + 1;

    beginResetModel();
    institution->setId(nextId);
    _institutions.append(institution);
    endResetModel();
    return nextId;
}

FinancialInstitution *FinancialInstitutionModel::addFinancialInstitution(const QString &name)
{
    auto *newFinancialInstitution = new FinancialInstitution(name);
    addFinancialInstitution(newFinancialInstitution);
    return newFinancialInstitution;
}

void FinancialInstitutionModel::reset()
{
    beginResetModel();
    _institutions.clear();
    endResetModel();
}

void FinancialInstitutionModel::removeFinancialInstitution(int id)
{
    beginResetModel();
    auto res = std::find_if(_institutions.cbegin(), _institutions.cend(), [&id] (const FinancialInstitution *institution){ return institution->getId() == id;});
    if (res != _institutions.cend())
        _institutions.erase(res);
    endResetModel();
}

void FinancialInstitutionModel::removeFinancialInstitution(FinancialInstitution *institution)
{
    beginResetModel();
    auto res = std::find_if(_institutions.cbegin(), _institutions.cend(), [&institution] (const FinancialInstitution *other){ return institution == other;});
    if (res != _institutions.cend())
        _institutions.erase(res);
    endResetModel();
}
