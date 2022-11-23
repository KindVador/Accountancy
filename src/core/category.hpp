#ifndef ACCOUNTANCY_CATEGORY_HPP
#define ACCOUNTANCY_CATEGORY_HPP

#include <QList>
#include <QObject>

class SubCategory;

class Category : public QObject
{
    Q_OBJECT

public:
    Category() = default;
    ~Category() override = default;

    void addSubCategory(const SubCategory* subCategory);
    bool removeSubCategory(const SubCategory* subCategory);
    bool removeSubCategoryByName(const QString& name);
    [[nodiscard]] QVector<const SubCategory*> subCategories() const;

private:
    QString _name;
    QList<const SubCategory*> _subCategories;
};


#endif//ACCOUNTANCY_CATEGORY_HPP
