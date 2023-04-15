#include "categoriestreedialog.hpp"
#include "controller/controller.hpp"
#include "createcategorydialog.hpp"
#include "ui_categoriestreedialog.h"

CategoriesTreeDialog::CategoriesTreeDialog(QWidget* parent, CategoryModel* model) : QDialog(parent), ui(new Ui::CategoriesTreeDialog), _model(model)
{
    ui->setupUi(this);
    setWindowTitle("Categories");

    // init TableView model
    ui->treeView->setModel(_model);

    // connect buttons
    connect(ui->addPushButton, &QPushButton::clicked, this, &CategoriesTreeDialog::onAddCategoryAction);
    connect(ui->removePushButton, &QPushButton::clicked, this, &CategoriesTreeDialog::onRemoveCategoryAction);
}

CategoriesTreeDialog::~CategoriesTreeDialog()
{
    delete ui;
}

void CategoriesTreeDialog::onAddCategoryAction()
{
    auto dlg = CreateCategoryDialog(this, _model);
    dlg.exec();
}

void CategoriesTreeDialog::onRemoveCategoryAction() const
{
    QList<QModelIndex> selectedIndexes = ui->treeView->selectionModel()->selectedIndexes();
    for (QModelIndex selectedIndex: selectedIndexes) {
        QUuid uid = _model->data(selectedIndex, Qt::UserRole).toUuid();
        Controller::instance()->removeCategory(uid);
    }
}