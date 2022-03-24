#include "createcurrencydialog.hpp"
#include "ui_CreateCurrencyDialog.h"
#include "../core/controller.hpp"


CreateCurrencyDialog::CreateCurrencyDialog(QWidget *parent) :
        QDialog(parent), ui(new Ui::CreateCurrencyDialog)
{
    ui->setupUi(this);
}

CreateCurrencyDialog::~CreateCurrencyDialog()
{
    delete ui;
}

void CreateCurrencyDialog::accept()
{
    QString name = ui->nameLineEdit->text();
    QString symbol = ui->symbolLineEdit->text();
    if (!name.isEmpty() && !symbol.isEmpty())
        Controller::getInstance()->addCurrency(name, symbol);

    QDialog::accept();
}

void CreateCurrencyDialog::reject()
{
    QDialog::reject();
}

