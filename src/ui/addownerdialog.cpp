#include "addownerdialog.hpp"
#include "ui_addOwnerDialog.h"
#include "../core/controller.hpp"


AddOwnerDialog::AddOwnerDialog(QWidget *parent) :
        QDialog(parent), ui(new Ui::AddOwnerDialog)
{
    ui->setupUi(this);
}

AddOwnerDialog::~AddOwnerDialog()
{
    delete ui;
}

void AddOwnerDialog::accept()
{
    Controller *controller = Controller::getInstance();
    controller->addOwner(ui->nameLineEdit->text(),
                         ui->warningBalanceDoubleSpinBox->value(),
                         ui->commentsTextEdit->toPlainText(),
                         ui->hideCheckBox->isChecked());
    QDialog::accept();
}

void AddOwnerDialog::reject()
{
    QDialog::reject();
}

