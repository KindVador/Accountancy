#include "addaccountdialog.hpp"
#include "ui_AddAccountDialog.h"
#include "../core/controller.hpp"


AddAccountDialog::AddAccountDialog(QWidget *parent) :
        QDialog(parent), ui(new Ui::AddAccountDialog)
{
    ui->setupUi(this);

    // Populate Account Type ComboBox
    for (auto it = STRING_2_ACCOUNT_TYPE.cbegin(); it != STRING_2_ACCOUNT_TYPE.cend(); ++it)
        ui->typeComboBox->addItem(it.key());

    // Populate Currency ComboBox
    ui->currencyComboBox->setModel(Model::getInstance()->getCurrencyModel());

    // Populate Institution ComboBox
    ui->institutionComboBox->setModel(Model::getInstance()->getFinancialInstitutionModel());

    // Populate Owners List
    ui->ownersListView->setModel(Model::getInstance()->getOwnerModel());
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

