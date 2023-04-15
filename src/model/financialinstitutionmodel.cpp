#include "financialinstitutionmodel.hpp"

#include <utility>

#include <QJsonArray>

constexpr const int ObjectRole = Qt::UserRole + 1;

FinancialInstitutionModel::FinancialInstitutionModel(QString name) : AbstractModel(std::move(name))
{
}

int FinancialInstitutionModel::rowCount(const QModelIndex& parent) const
{
    return (int) _institutions.count();
}

QVariant FinancialInstitutionModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= _institutions.count() || index.row() < 0)
        return {};

    QVariant v;
    switch (role) {
        case Qt::DisplayRole:
            v.setValue(_institutions.at(index.row())->getName());
            break;
        case Qt::UserRole:
            v.setValue(_institutions.at(index.row())->getUid());
            break;
        case ObjectRole:
            v.setValue(_institutions.at(index.row()));
            break;
        default:
            break;
    }
    return v;
}

void FinancialInstitutionModel::addFinancialInstitution(FinancialInstitution* institution)
{
    if (institution == nullptr)
        return;

    beginResetModel();
    _institutions.append(institution);
    endResetModel();
}

FinancialInstitution* FinancialInstitutionModel::addFinancialInstitution(const QString& name)
{
    auto* newFinancialInstitution = new FinancialInstitution(name);
    addFinancialInstitution(newFinancialInstitution);
    return newFinancialInstitution;
}

void FinancialInstitutionModel::reset()
{
    beginResetModel();
    _institutions.clear();
    endResetModel();
}

void FinancialInstitutionModel::removeFinancialInstitution(QUuid uid)
{
    beginResetModel();
    auto res = std::find_if(_institutions.cbegin(), _institutions.cend(), [&uid](const FinancialInstitution* institution) { return institution->getUid() == uid; });
    if (res != _institutions.cend())
        _institutions.erase(res);
    endResetModel();
}

void FinancialInstitutionModel::removeFinancialInstitution(const FinancialInstitution* institution)
{
    beginResetModel();
    auto res = std::find_if(_institutions.cbegin(), _institutions.cend(), [&institution](const FinancialInstitution* other) { return institution == other; });
    if (res != _institutions.cend())
        _institutions.erase(res);
    endResetModel();
}

FinancialInstitution* FinancialInstitutionModel::getFinancialInstitution(QUuid uid) const
{
    auto res = std::find_if(_institutions.cbegin(), _institutions.cend(), [&uid](const FinancialInstitution* other) { return uid ==
                                                                                                                             other->getUid(); });
    if (res != _institutions.cend())
        return *res;
    return nullptr;
}

bool FinancialInstitutionModel::isDirty() const
{
    // TODO implement FinancialInstitutionModel::isDirty()
    return false;
}

void FinancialInstitutionModel::write(QJsonObject& json) const
{
    QJsonArray institutions;
    for (int i = 0; i < rowCount(QModelIndex()); ++i) {
        const FinancialInstitution* institution = data(index(i, 0), ObjectRole).value<FinancialInstitution*>();
        QJsonObject institutionJson;
        institution->write(institutionJson);
        institutions.append(institutionJson);
    }
    json[getName()] = institutions;
}

void FinancialInstitutionModel::read(const QJsonObject& json)
{
    if (json.contains(getName()) && json[getName()].isArray()) {
        QJsonArray institutionsJsonArray = json[getName()].toArray();
        for (const QJsonValueConstRef& institution: qAsConst(institutionsJsonArray))
            addFinancialInstitution(FinancialInstitution::fromJson(institution.toObject()));
    }
}
