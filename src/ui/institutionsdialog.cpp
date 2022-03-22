#include "institutionsdialog.hpp"
#include "ui_InstitutionsDialog.h"


InstitutionsDialog::InstitutionsDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::InstitutionsDialog)
{
    ui->setupUi(this);
}

InstitutionsDialog::~InstitutionsDialog()
{
    delete ui;
}

void InstitutionsDialog::accept()
{
    QDialog::accept();
}

void InstitutionsDialog::reject()
{
    QDialog::reject();
}
