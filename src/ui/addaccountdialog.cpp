#include "addaccountdialog.hpp"
#include "ui_AddAccountDialog.h"
#include "../core/controller.hpp"

constexpr const int ObjectRole = Qt::UserRole + 1;

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

    // make connections
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AddAccountDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &AddAccountDialog::reject);
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
    QString accountNumber = ui->numberLineEdit->text();
    auto institution = ui->institutionComboBox->currentData().value<FinancialInstitution*>();
    AccountType type = STRING_2_ACCOUNT_TYPE[ui->typeComboBox->currentText()];
    auto currency = ui->currencyComboBox->currentData().value<Currency*>();
    QList<const Owner *> owners = getSelectedOwners();
    auto initialBalance = (float)ui->initialBalanceDoubleSpinBox->value();
    auto warningBalance = (float)ui->warningBalanceDoubleSpinBox->value();
    QString comment = ui->commentsTextEdit->toPlainText();
    bool isIncludedInTotal = ui->includeInTotalCheckBox->checkState();
    bool isHidden = ui->hideCheckBox->checkState();
    controller->addAccount(institution, type, currency, owners, initialBalance, warningBalance, accountNumber, comment,
                           isIncludedInTotal, isHidden);
    QDialog::accept();
}

QList<const Owner *> AddAccountDialog::getSelectedOwners() const
{
    QList<const Owner *> selectedOwners;
    auto ownerModel = dynamic_cast<OwnerModel*>(ui->ownersListView->model());
    if (ownerModel == nullptr)
        return selectedOwners;
    QList<QModelIndex> selectedIndexes = ui->ownersListView->selectionModel()->selectedIndexes();
    for (const QModelIndex &selectedIndex : selectedIndexes)
        selectedOwners.append(ownerModel->data(selectedIndex, ObjectRole).value<Owner*>());

    return selectedOwners;
}
