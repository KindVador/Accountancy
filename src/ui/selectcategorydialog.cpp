// You may need to build the project (run Qt uic code generator) to get "ui_SelectCategoryDialog.h" resolved

#include "selectcategorydialog.hpp"
#include "ui_SelectCategoryDialog.h"


SelectCategoryDialog::SelectCategoryDialog(QWidget* parent, CategoryModel* model) : QDialog(parent), ui(new Ui::SelectCategoryDialog), _model(model)
{
    ui->setupUi(this);

    // init TableView model
    ui->treeView->setModel(_model);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SelectCategoryDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &SelectCategoryDialog::reject);
}

SelectCategoryDialog::~SelectCategoryDialog()
{
    delete ui;
}

void SelectCategoryDialog::accept()
{
    auto selection_model = ui->treeView->selectionModel();
    auto selected_indexes = selection_model->selectedIndexes();
    _selected_category = static_cast<Category*>(selected_indexes.first().internalPointer());
    QDialog::accept();
}

void SelectCategoryDialog::reject()
{
    _selected_category = nullptr;
    QDialog::reject();
}
const Category* SelectCategoryDialog::getSelectedCategory() const
{
    return _selected_category;
}
