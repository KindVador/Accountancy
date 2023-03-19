#include "categoriesdialog.hpp"
#include "controller/controller.hpp"
#include "createcategorydialog.hpp"
#include "ui_categoriesdialog.h"

CategoriesDialog::CategoriesDialog(QWidget* parent, CategoryModel* model) : QDialog(parent), ui(new Ui::CategoriesDialog), _model(model)
{
    ui->setupUi(this);

    // init TableView model
    ui->tableView->setModel(_model);

    // connect buttons
    connect(ui->addPushButton, &QPushButton::clicked, this, &CategoriesDialog::onAddCategoryAction);
    connect(ui->removePushButton, &QPushButton::clicked, this, &CategoriesDialog::onRemoveCategoryAction);
}

CategoriesDialog::~CategoriesDialog()
{
    delete ui;
}

void CategoriesDialog::accept()
{
    QDialog::accept();
}

void CategoriesDialog::reject()
{
    QDialog::reject();
}

void CategoriesDialog::onAddCategoryAction()
{
    auto dlg = CreateCategoryDialog(this);
    dlg.exec();
}

void CategoriesDialog::onRemoveCategoryAction() const
{
    QList<QModelIndex> selectedIndexes = ui->tableView->selectionModel()->selectedIndexes();
    for (QModelIndex selectedIndex: selectedIndexes) {
        QUuid uid = _model->data(selectedIndex, Qt::UserRole).toUuid();
        Controller::instance()->removeCategory(uid);
    }
}