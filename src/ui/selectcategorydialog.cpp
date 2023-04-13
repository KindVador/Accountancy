// You may need to build the project (run Qt uic code generator) to get "ui_SelectCategoryDialog.h" resolved

#include "selectcategorydialog.hpp"
#include "ui_SelectCategoryDialog.h"

SelectCategoryDialog::SelectCategoryDialog(QWidget* parent, CategoryModel* model) : QDialog(parent), ui(new Ui::SelectCategoryDialog), _model(model)
{
    ui->setupUi(this);

    // init TableView model
    _filter->setSourceModel(_model);
    _filter->setFilterCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    ui->treeView->setModel(_filter.get());

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SelectCategoryDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &SelectCategoryDialog::reject);
    connect(ui->lineEdit, &QLineEdit::textChanged, this, &SelectCategoryDialog::onFilterChanged);
    connect(ui->treeView, &QTreeView::doubleClicked, this, &SelectCategoryDialog::accept);
}

SelectCategoryDialog::~SelectCategoryDialog()
{
    delete ui;
}

void SelectCategoryDialog::accept()
{
    auto selection_model = ui->treeView->selectionModel();
    auto selected_index = _filter->mapToSource(selection_model->selectedIndexes().first());
    _selected_category = static_cast<Category*>(selected_index.internalPointer());
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

void SelectCategoryDialog::onFilterChanged(const QString& text)
{
    _filter->setFilterFixedString(text);
    if (text == "")
        return;

    ui->treeView->expandAll();
}
