#ifndef ACCOUNTANCY_SUBCATEGORY_HPP
#define ACCOUNTANCY_SUBCATEGORY_HPP

#include <QObject>

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
    void setParent1(const Category* parent);

private:
    QString _name;
    const Category* _parent = nullptr;
};

#endif//ACCOUNTANCY_SUBCATEGORY_HPP