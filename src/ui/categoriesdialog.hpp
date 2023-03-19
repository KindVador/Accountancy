#ifndef ACCOUNTANCY_CATEGORIESDIALOG_HPP
#define ACCOUNTANCY_CATEGORIESDIALOG_HPP

#include <QDialog>

#include "model/categorymodel.hpp"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class CategoriesDialog;
}
QT_END_NAMESPACE

class CategoriesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CategoriesDialog(QWidget* parent, CategoryModel* model);
    ~CategoriesDialog() override;

public slots:
    void accept() override;
    void reject() override;

private slots:
    void onAddCategoryAction();
    void onRemoveCategoryAction() const;

private:
    Ui::CategoriesDialog* ui;
    CategoryModel* _model = nullptr;
};


#endif//ACCOUNTANCY_CATEGORIESDIALOG_HPP
