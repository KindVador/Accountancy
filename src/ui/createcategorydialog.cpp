#include "createcategorydialog.hpp"
#include "controller/controller.hpp"
#include "ui_createcategorydialog.h"

CreateCategoryDialog::CreateCategoryDialog(QWidget* parent) : QDialog(parent), ui(new Ui::CreateCategoryDialog)
{
    ui->setupUi(this);
}

CreateCategoryDialog::~CreateCategoryDialog()
{
    delete ui;
}

void CreateCategoryDialog::accept()
{
    QString name = ui->nameLineEdit->text();
    if (!name.isEmpty())
        Controller::instance()->addCategory(name);

    QDialog::accept();
}

void CreateCategoryDialog::reject()
{
    QDialog::reject();
}
