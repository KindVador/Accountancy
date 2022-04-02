#include "institutionsdialog.hpp"
#include "ui_InstitutionsDialog.h"
#include "createinstitutiondialog.hpp"
#include "../core/controller.hpp"

InstitutionsDialog::InstitutionsDialog(QWidget *parent, FinancialInstitutionModel *model) :
        QDialog(parent),
        ui(new Ui::InstitutionsDialog),
        _model(model)
{
    ui->setupUi(this);

    // init TableView model
    ui->tableView->setModel(_model);

    // connect buttons
    connect(ui->addPushButton, &QPushButton::clicked, this, &InstitutionsDialog::onAddInstitutionAction);
    connect(ui->removePushButton, &QPushButton::clicked, this, &InstitutionsDialog::onRemoveInstitutionAction);

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

void InstitutionsDialog::onAddInstitutionAction()
{
    CreateInstitutionDialog dlg = CreateInstitutionDialog(this);
    dlg.exec();
}

void InstitutionsDialog::onRemoveInstitutionAction()
{
    QList<QModelIndex> selectedIndexes = ui->tableView->selectionModel()->selectedIndexes();
    for (QModelIndex selectedIndex : selectedIndexes) {
        int id = _model->data(selectedIndex, Qt::UserRole).toInt();
        Controller::getInstance()->removeInstitution(id);
    }
}
