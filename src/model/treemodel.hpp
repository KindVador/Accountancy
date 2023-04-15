#ifndef ACCOUNTANCY_TREEMODEL_HPP
#define ACCOUNTANCY_TREEMODEL_HPP

#include <QAbstractItemModel>

#include "treeitem.hpp"

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TreeModel(const QString& data, QObject* parent = nullptr);
    ~TreeModel() override;

    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QModelIndex parent(const QModelIndex& index) const override;
    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex& parent = QModelIndex()) const override;

private:
    void setupModelData(const QStringList& lines, TreeItem* parent);

    TreeItem* _rootItem;
};

#endif//ACCOUNTANCY_TREEMODEL_HPP
