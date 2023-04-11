// You may need to build the project (run Qt uic code generator) to get "ui_SelectCategoryDialog.h" resolved

#include "selectcategorydialog.hpp"
#include "ui_SelectCategoryDialog.h"


SelectCategoryDialog::SelectCategoryDialog(QWidget* parent, CategoryModel* model) : QDialog(parent), ui(new Ui::SelectCategoryDialog), _model(model)
{
    ui->setupUi(this);

    // init TableView model
    ui->treeView->setModel(_model);
}

SelectCategoryDialog::~SelectCategoryDialog()
{
    delete ui;
}

void SelectCategoryDialog::accept()
{
    QDialog::accept();
}

void SelectCategoryDialog::reject()
{
    QDialog::reject();
}
