#ifndef ACCOUNTANCY_CATEGORYMODEL_HPP
#define ACCOUNTANCY_CATEGORYMODEL_HPP

#include <QAbstractListModel>

#include "category.hpp"

class CategoryModel : public QAbstractListModel
{
    Q_OBJECT

public:
    CategoryModel() = default;
    ~CategoryModel() override = default;

    // QAbstractListModel interface
    [[nodiscard]] int rowCount(const QModelIndex& parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;

    void addCategory(Category* category);
    Category* addCategory(const QString& name);
    void removeCategory(const Category* category);
    void removeCategory(const QUuid& uid);
    [[nodiscard]] Category* getCategory(const QUuid& uid) const;

    void reset();

private:
    QList<Category*> _categories;
};


#endif//ACCOUNTANCY_CATEGORYMODEL_HPP
