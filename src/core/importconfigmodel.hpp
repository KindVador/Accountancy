#ifndef ACCOUNTANCY_IMPORTCONFIGMODEL_HPP
#define ACCOUNTANCY_IMPORTCONFIGMODEL_HPP

#include <QAbstractListModel>

#include "importconfig.hpp"

class ImportConfigModel : public QAbstractListModel
{
    Q_OBJECT

public:
    ImportConfigModel() = default;
    ~ImportConfigModel() override = default;

    // QAbstractListModel interface
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    void addImportConfig(ImportConfig *config);
    void removeImportConfig(const QModelIndex& index);

    [[nodiscard]] const QList<ImportConfig*>& importConfigs() const;

    void reset();

private:
    QList<ImportConfig *> _importConfigs;
};

#endif //ACCOUNTANCY_IMPORTCONFIGMODEL_HPP
