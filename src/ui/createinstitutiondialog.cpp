#include "createinstitutiondialog.hpp"
#include "ui_CreateInstitutionDialog.h"
#include "../core/controller.hpp"


CreateInstitutionDialog::CreateInstitutionDialog(QWidget *parent) :
        QDialog(parent), ui(new Ui::CreateInstitutionDialog)
{
    ui->setupUi(this);
}

CreateInstitutionDialog::~CreateInstitutionDialog()
{
    delete ui;
}

void CreateInstitutionDialog::accept()
{
    QString name = ui->nameLineEdit->text();
    if (!name.isEmpty())
        Controller::getInstance()->addFinancialInstitution(name);

    QDialog::accept();
}

void CreateInstitutionDialog::reject()
{
    QDialog::reject();
}

