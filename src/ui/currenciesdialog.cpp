#include "currenciesdialog.hpp"
#include "ui_CurrenciesDialog.h"

CurrenciesDialog::CurrenciesDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::CurrenciesDialog)
{
    ui->setupUi(this);
}

CurrenciesDialog::~CurrenciesDialog()
{
    delete ui;
}

void CurrenciesDialog::accept()
{
    QDialog::accept();
}

void CurrenciesDialog::reject()
{
    QDialog::reject();
}
