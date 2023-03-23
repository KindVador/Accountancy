#include "ownermodel.hpp"

#include <utility>

#include <QJsonArray>

constexpr const int ObjectRole = Qt::UserRole + 1;

OwnerModel::OwnerModel(QString name) : AbstractModel(std::move(name))
{
}

void OwnerModel::addOwner(Owner* owner)
{
    if (owner == nullptr)
        return;
    qDebug() << "Owner model, add Owner: " << owner->getName();
    beginResetModel();
    _owners.append(owner);
    endResetModel();
}

Owner* OwnerModel::addOwner(const QString& name, double warningBalance, const QString& comment, bool isHidden)
{
    qDebug() << "Owner model, add Owner: " << name;
    auto* newOwner = new Owner(name, warningBalance, comment, isHidden);
    addOwner(newOwner);
    return newOwner;
}

Owner* OwnerModel::getOwner(const QString& name) const
{
    auto ownerIt = std::find_if(_owners.cbegin(), _owners.cend(), [&name](const Owner* owner) { return owner->getName() == name; });
    // case NOT FOUND
    if (ownerIt == _owners.end())
        return nullptr;

    return *ownerIt;
}

int OwnerModel::rowCount(const QModelIndex& parent) const
{
    return (int) _owners.count();
}

QVariant OwnerModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= _owners.count() || index.row() < 0)
        return {};

    QVariant v;
    switch (role) {
        case Qt::DisplayRole:
            v.setValue(_owners.at(index.row())->getName());
            break;
        case Qt::UserRole:
            v.setValue(_owners.at(index.row())->getUid());
            break;
        case ObjectRole:
            v.setValue(_owners.at(index.row()));
            break;
        default:
            break;
    }

    return v;
}

void OwnerModel::reset()
{
    beginResetModel();
    _owners.clear();
    endResetModel();
}

void OwnerModel::removeOwner(const QModelIndex& index)
{
    beginResetModel();
    delete _owners.at(index.row());
    _owners.removeAt(index.row());
    endResetModel();
}

Owner* OwnerModel::getOwner(QUuid uid) const
{
    auto ownerIt = std::find_if(_owners.cbegin(), _owners.cend(), [&uid](const Owner* owner) { return owner->getUid() == uid; });
    // case NOT FOUND
    if (ownerIt == _owners.end())
        return nullptr;

    return *ownerIt;
}

bool OwnerModel::isDirty() const
{
    // TODO implement OwnerModel::isDirty()
    return false;
}

void OwnerModel::write(QJsonObject& json) const
{
    QJsonArray owners;
    for (int i = 0; i < rowCount(QModelIndex()); ++i) {
        const Owner* owner = data(index(i, 0), ObjectRole).value<Owner*>();
        QJsonObject ownerJson;
        owner->write(ownerJson);
        owners.append(ownerJson);
    }
    json[getName()] = owners;
}

void OwnerModel::read(const QJsonObject& json)
{
    if (json.contains(getName()) && json[getName()].isArray()) {
        QJsonArray ownersJsonArray = json[getName()].toArray();
        for (const QJsonValueConstRef& owner: qAsConst(ownersJsonArray))
            addOwner(Owner::fromJson(owner.toObject()));
    }
}
