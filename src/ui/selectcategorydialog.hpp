#ifndef ACCOUNTANCY_SELECTCATEGORYDIALOG_HPP
#define ACCOUNTANCY_SELECTCATEGORYDIALOG_HPP

#include <QDialog>

#include "model/categorymodel.hpp"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class SelectCategoryDialog;
}
QT_END_NAMESPACE

class SelectCategoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectCategoryDialog(QWidget* parent, CategoryModel* model);
    ~SelectCategoryDialog() override;

    [[nodiscard]] const Category* getSelectedCategory() const;

public slots:
    void accept() override;
    void reject() override;

private:
    Ui::SelectCategoryDialog* ui;
    CategoryModel* _model = nullptr;
    const Category* _selected_category = nullptr;
};


#endif//ACCOUNTANCY_SELECTCATEGORYDIALOG_HPP
