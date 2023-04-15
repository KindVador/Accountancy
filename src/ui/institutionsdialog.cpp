#include "institutionsdialog.hpp"
#include "controller/controller.hpp"
#include "createinstitutiondialog.hpp"
#include "ui_institutionsdialog.h"

InstitutionsDialog::InstitutionsDialog(QWidget* parent, FinancialInstitutionModel* model) : QDialog(parent),
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
    auto dlg = CreateInstitutionDialog(this);
    dlg.exec();
}

void InstitutionsDialog::onRemoveInstitutionAction() const
{
    QList<QModelIndex> selectedIndexes = ui->tableView->selectionModel()->selectedIndexes();
    for (QModelIndex selectedIndex: selectedIndexes) {
        QUuid uid = _model->data(selectedIndex, Qt::UserRole).toUuid();
        Controller::instance()->removeInstitution(uid);
    }
}
