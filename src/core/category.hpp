#ifndef ACCOUNTANCY_CATEGORY_HPP
#define ACCOUNTANCY_CATEGORY_HPP

#include <QList>
#include <QObject>
#include <QUuid>

#include "Interfaces/iserializable.hpp"

class SubCategory;

class Category : public QObject, public ISerializable
{
    Q_OBJECT

public:
    Category() = default;
    ~Category() override = default;

    void addSubCategory(const SubCategory* subCategory);
    bool removeSubCategory(const SubCategory* subCategory);
    bool removeSubCategoryByName(const QString& name);
    [[nodiscard]] QVector<const SubCategory*> subCategories() const;

    // Serialization
    void read(const QJsonObject& json) override;
    void write(QJsonObject& json) const override;

private:
    QUuid _uid;
    QString _name;
    QList<const SubCategory*> _subCategories;
};


#endif//ACCOUNTANCY_CATEGORY_HPP
