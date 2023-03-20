#ifndef ACCOUNTANCY_CATEGORYMODEL_HPP
#define ACCOUNTANCY_CATEGORYMODEL_HPP

#include <QAbstractItemModel>

#include "abstractmodel.hpp"
#include "core/category.hpp"


// TODO implement a tree model for categories
// https://doc.qt.io/qt-6/qtwidgets-itemviews-simpletreemodel-example.html

/**
 * @brief Category model based on a Tree structure.
 */
class CategoryModel : public QAbstractItemModel, public AbstractModel
{
    Q_OBJECT

public:
    explicit CategoryModel(QString name);
    ~CategoryModel() override = default;

    // AbstractModel
    [[nodiscard]] bool isDirty() const override;
    void reset() override;
    void write(QJsonObject& json) const override;
    void read(const QJsonObject& json) override;

    // QAbstractListModel interface
    [[nodiscard]] int rowCount(const QModelIndex& parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    [[nodiscard]] QModelIndex parent(const QModelIndex& child) const override;
    [[nodiscard]] int columnCount(const QModelIndex& parent) const override;

    /**
     * @brief
     * @param category
     * @param parent
     */
    void addCategory(Category* category, Category* parent = nullptr);

    /**
     * @brief
     * @param name
     * @param parent
     * @return
     */
    Category* addCategory(const QString& name, Category* parent = nullptr);

    /**
     * @brief
     * @param uid
     */
    void removeCategory(const QUuid& uid);

    /**
     * @brief
     * @param name
     */
    void removeCategory(const QString& name);

    /**
     * @brief
     * @param name
     * @return
     */
    [[nodiscard]] Category* category(const QString& name) const;

    /**
     * @brief Search in the tree model a category based on its UID
     * @param uid of the searched category
     * @return Category* if found otherwise nullptr
     */
    [[nodiscard]] Category* category(const QUuid& uid) const;


private:
    std::unique_ptr<Category> _rootCategory = std::make_unique<Category>();
};


#endif//ACCOUNTANCY_CATEGORYMODEL_HPP
