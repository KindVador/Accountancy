#include "importconfigmodel.hpp"

#include <utility>

constexpr const int ObjectRole = Qt::UserRole + 1;

ImportConfigModel::ImportConfigModel(QString name) : AbstractModel(std::move(name))
{
}

int ImportConfigModel::rowCount(const QModelIndex& parent) const
{
    return (int) _importConfigs.count();
}

QVariant ImportConfigModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= _importConfigs.count() || index.row() < 0)
        return {};

    QVariant v;
    switch (role) {
        case Qt::DisplayRole:
            v.setValue(_importConfigs.at(index.row())->getName());
            break;
        case Qt::UserRole:
            v.setValue(_importConfigs.at(index.row())->getUid());
            break;
        case ObjectRole:
            v.setValue(_importConfigs.at(index.row()));
            break;
        default:
            break;
    }
    return v;
}

void ImportConfigModel::addImportConfig(ImportConfig* config)
{
    if (config == nullptr)
        return;

    _importConfigs.append(config);
}

void ImportConfigModel::removeImportConfig(const QModelIndex& index)
{
    beginResetModel();
    delete _importConfigs.at(index.row());
    _importConfigs.removeAt(index.row());
    endResetModel();
}

const QList<ImportConfig*>& ImportConfigModel::importConfigs() const
{
    return _importConfigs;
}

void ImportConfigModel::reset()
{
    beginResetModel();
    _importConfigs.clear();
    endResetModel();
}

bool ImportConfigModel::isDirty() const
{
    // TODO implement ImportConfigModel::isDirty()
    return false;
}
