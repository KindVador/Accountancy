#include "createinstitutiondialog.hpp"
#include "controller/controller.hpp"
#include "ui_createinstitutiondialog.h"


CreateInstitutionDialog::CreateInstitutionDialog(QWidget* parent) : QDialog(parent), ui(new Ui::CreateInstitutionDialog)
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
        Controller::instance()->addFinancialInstitution(name);

    QDialog::accept();
}

void CreateInstitutionDialog::reject()
{
    QDialog::reject();
}
