#ifndef ACCOUNTANCY_CATEGORY_HPP
#define ACCOUNTANCY_CATEGORY_HPP

#include <QList>
#include <QUuid>

#include "Interfaces/iserializable.hpp"

class SubCategory;

class Category : public ISerializable
{

public:
    Category();
    explicit Category(QString name);
    ~Category() override = default;

    // Getter & Setter
    [[nodiscard]] const QUuid& getUid() const;
    void setUid(const QUuid& uid);
    [[nodiscard]] const QString& getName() const;
    void setName(const QString& name);

    // Public API for SubCategories
    void addSubCategory(const SubCategory* subCategory);
    bool removeSubCategory(const SubCategory* subCategory);
    bool removeSubCategoryByName(const QString& name);
    [[nodiscard]] QVector<const SubCategory*> subCategories() const;

    // Serialization
    void read(const QJsonObject& json) override;
    void write(QJsonObject& json) const override;

protected:
    QUuid _uid;
    QString _name;

private:
    QList<const SubCategory*> _subCategories;
};

#endif//ACCOUNTANCY_CATEGORY_HPP
