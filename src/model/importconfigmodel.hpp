#ifndef ACCOUNTANCY_IMPORTCONFIGMODEL_HPP
#define ACCOUNTANCY_IMPORTCONFIGMODEL_HPP

#include <QAbstractListModel>

#include "abstractmodel.hpp"
#include "core/importconfig.hpp"

class ImportConfigModel : public QAbstractListModel, public AbstractModel
{
    Q_OBJECT

public:
    explicit ImportConfigModel(QString name);
    ~ImportConfigModel() override = default;

    // AbstractModel
    [[nodiscard]] bool isDirty() const override;
    void reset() override;
    void write(QJsonObject& json) const override;
    void read(const QJsonObject& json) override;

    // QAbstractListModel interface
    [[nodiscard]] int rowCount(const QModelIndex& parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;

    void addImportConfig(ImportConfig* config);
    void removeImportConfig(const QModelIndex& index);

    [[nodiscard]] const QList<ImportConfig*>& importConfigs() const;

private:
    QList<ImportConfig*> _importConfigs;
};

#endif//ACCOUNTANCY_IMPORTCONFIGMODEL_HPP
