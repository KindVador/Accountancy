#include "addaccountdialog.hpp"
#include "ui_AddAccountDialog.h"
#include "../core/controller.hpp"
#include "../core/account.hpp"


AddAccountDialog::AddAccountDialog(QWidget *parent) :
        QDialog(parent), ui(new Ui::AddAccountDialog)
{
    ui->setupUi(this);

    // Populate Account Type ComboBox
    for (auto it = STRING_2_ACCOUNT_TYPE.cbegin(); it != STRING_2_ACCOUNT_TYPE.cend(); ++it)
        ui->typeComboBox->addItem(it.key());

    // TODO: Populate Currency ComboBox

    // TODO: Populate Institution ComboBox

    // TODO: Populate Owners List
}

AddAccountDialog::~AddAccountDialog()
{
    delete ui;
}

void AddAccountDialog::reject()
{
    QDialog::reject();
}

void AddAccountDialog::accept()
{
    Controller *controller = Controller::getInstance();
    // TODO: controller->addAccount();
    QDialog::accept();
}

