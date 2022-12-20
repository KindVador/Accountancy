#ifndef ACCOUNTANCY_SUBCATEGORY_HPP
#define ACCOUNTANCY_SUBCATEGORY_HPP

#include <QObject>

#include "Interfaces/iserializable.hpp"
#include "category.hpp"

class SubCategory : public Category
{
    Q_OBJECT

public:
    SubCategory() = default;
    ~SubCategory() override = default;

    [[nodiscard]] const QString& getName() const;
    void setName(const QString& name);

    [[nodiscard]] const Category* getParent() const;
    void setParent(const Category* parent);

    // Serialization
    void read(const QJsonObject& json) override;
    void write(QJsonObject& json) const override;

private:
    QString _name;
    const Category* _parent = nullptr;
};

#endif//ACCOUNTANCY_SUBCATEGORY_HPP
