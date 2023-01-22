#ifndef ACCOUNTANCY_CATEGORY_HPP
#define ACCOUNTANCY_CATEGORY_HPP

#include <QList>
#include <QUuid>

#include "Interfaces/iserializable.hpp"

class Category : public ISerializable
{

public:
    Category();
    explicit Category(QString name);
    ~Category() override = default;

    static Category* fromJson(const QJsonObject& json);

    // Getter & Setter
    [[nodiscard]] const QUuid& getUid() const;
    void setUid(const QUuid& uid);
    [[nodiscard]] const QString& getName() const;
    void setName(const QString& name);

    // Public API for SubCategories
    void addSubCategory(const Category* subCategory);
    bool removeSubCategory(const Category* subCategory);
    bool removeSubCategoryByName(const QString& name);
    [[nodiscard]] QVector<const Category*> subCategories() const;

    // Serialization
    void read(const QJsonObject& json) override;
    void write(QJsonObject& json) const override;

private:
    QUuid _uid;
    QString _name;
    QList<const Category*> _subCategories;
};

#endif//ACCOUNTANCY_CATEGORY_HPP
