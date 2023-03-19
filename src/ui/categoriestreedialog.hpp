#ifndef ACCOUNTANCY_CATEGORIESTREEDIALOG_HPP
#define ACCOUNTANCY_CATEGORIESTREEDIALOG_HPP

#include <QDialog>

#include "model/categorymodel.hpp"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class CategoriesTreeDialog;
}
QT_END_NAMESPACE

class CategoriesTreeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CategoriesTreeDialog(QWidget* parent, CategoryModel* model);
    ~CategoriesTreeDialog() override;

public slots:
    void accept() override;
    void reject() override;

private slots:
    void onAddCategoryAction();
    void onRemoveCategoryAction() const;

private:
    Ui::CategoriesTreeDialog* ui;
    CategoryModel* _model = nullptr;
};


#endif//ACCOUNTANCY_CATEGORIESTREEDIALOG_HPP
