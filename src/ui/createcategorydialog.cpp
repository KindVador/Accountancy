#include "createcategorydialog.hpp"
#include "controller/controller.hpp"
#include "ui_createcategorydialog.h"

CreateCategoryDialog::CreateCategoryDialog(QWidget* parent, CategoryModel* model) : QDialog(parent), ui(new Ui::CreateCategoryDialog), _model(model)
{
    ui->setupUi(this);

    // Populate and configure parent tree view with categories
    ui->parentTreeView->setModel(_model);
    ui->parentTreeView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    ui->parentTreeView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
}

CreateCategoryDialog::~CreateCategoryDialog()
{
    delete ui;
}

void CreateCategoryDialog::accept()
{
    QString name = ui->nameLineEdit->text();
    Category* parent = nullptr;
    if (ui->parentGroupBox->isChecked()) {
        auto selection_model = ui->parentTreeView->selectionModel();
        auto selected_index = selection_model->selectedIndexes().first();
        parent = static_cast<Category*>(selected_index.internalPointer());
    }

    if (!name.isEmpty())
        Controller::instance()->addCategory(name, parent);

    QDialog::accept();
}

void CreateCategoryDialog::reject()
{
    ui->nameLineEdit->clear();
    QDialog::reject();
}
