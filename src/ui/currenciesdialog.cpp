#include "currenciesdialog.hpp"
#include "ui_CurrenciesDialog.h"

CurrenciesDialog::CurrenciesDialog(QWidget *parent, CurrencyModel *model) :
        QDialog(parent), ui(new Ui::CurrenciesDialog), _model(model)
{
    ui->setupUi(this);

    // init TableView model
    ui->tableView->setModel(_model);
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
