#ifndef ACCOUNTANCY_CREATECATEGORYDIALOG_HPP
#define ACCOUNTANCY_CREATECATEGORYDIALOG_HPP

#include <QDialog>

#include "model/categorymodel.hpp"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class CreateCategoryDialog;
}
QT_END_NAMESPACE

class CreateCategoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateCategoryDialog(QWidget* parent, CategoryModel* model);

    ~CreateCategoryDialog() override;

public slots:
    void accept() override;
    void reject() override;

private:
    Ui::CreateCategoryDialog* ui;
    CategoryModel* _model = nullptr;
};


#endif//ACCOUNTANCY_CREATECATEGORYDIALOG_HPP
