#ifndef ACCOUNTANCY_CATEGORY_HPP
#define ACCOUNTANCY_CATEGORY_HPP

#include <QList>
#include <QUuid>

#include "Interfaces/iserializable.hpp"

class Category : public ISerializable
{

public:
    Category();
    explicit Category(QString name, Category* parent = nullptr);
    ~Category() override = default;

    static Category* fromJson(const QJsonObject& json);

    // Getter & Setter
    [[nodiscard]] const QUuid& getUid() const;
    void setUid(const QUuid& uid);
    [[nodiscard]] const QString& getName() const;
    void setName(const QString& name);
    [[nodiscard]] Category* getParentItem() const;
    void setParentItem(Category* parentItem);

    // Public API for SubCategories
    void addSubCategory(Category* subCategory);
    bool removeSubCategory(Category* subCategory);
    bool removeSubCategoryByName(const QString& name);
    [[nodiscard]] QVector<Category*>& subCategories();
    void removeAllSubCategories();

    // Serialization
    void read(const QJsonObject& json) override;
    void write(QJsonObject& json) const override;

    // Tree item
    [[nodiscard]] Category* subCategory(int row) const;
    [[nodiscard]] int childCount() const;
    [[nodiscard]] int columnCount() const;
    [[nodiscard]] QVariant data(int column) const;
    [[nodiscard]] int row() const;
    [[nodiscard]] Category* parentItem() const;

private:
    QUuid _uid;
    QString _name;
    QVector<Category*> _subCategories;
    Category* _parentItem = nullptr;
};

#endif//ACCOUNTANCY_CATEGORY_HPP
