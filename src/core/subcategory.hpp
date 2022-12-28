#ifndef ACCOUNTANCY_SUBCATEGORY_HPP
#define ACCOUNTANCY_SUBCATEGORY_HPP

#include "Interfaces/iserializable.hpp"
#include "category.hpp"

class SubCategory : public Category
{

public:
    SubCategory() = default;
    explicit SubCategory(QString name, const Category* parent);
    ~SubCategory() override = default;

    [[nodiscard]] QUuid getParent() const;
    void setParent(const QUuid& parent);

    // Serialization
    void read(const QJsonObject& json) override;
    void write(QJsonObject& json) const override;

private:
    QUuid _parentUid;
};

#endif//ACCOUNTANCY_SUBCATEGORY_HPP
