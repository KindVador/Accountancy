#include "currenciesdialog.hpp"
#include "ui_CurrenciesDialog.h"
#include "createcurrencydialog.hpp"
#include "../core/controller.hpp"

CurrenciesDialog::CurrenciesDialog(QWidget *parent, CurrencyModel *model) :
        QDialog(parent), ui(new Ui::CurrenciesDialog), _model(model)
{
    ui->setupUi(this);

    // init TableView model
    ui->tableView->setModel(_model);

    // connect buttons
    connect(ui->addPushButton, &QPushButton::clicked, this, &CurrenciesDialog::onAddCurrencyAction);
    connect(ui->removePushButton, &QPushButton::clicked, this, &CurrenciesDialog::onRemoveCurrencyAction);

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

void CurrenciesDialog::onAddCurrencyAction()
{
    CreateCurrencyDialog dlg = CreateCurrencyDialog(this);
    dlg.exec();
}

void CurrenciesDialog::onRemoveCurrencyAction()
{
    QList<QModelIndex> selectedIndexes = ui->tableView->selectionModel()->selectedIndexes();
    for (QModelIndex selectedIndex : selectedIndexes) {
        QUuid uid = _model->data(selectedIndex, Qt::UserRole).toUuid();
        Controller::instance()->removeCurrency(uid);
    }
}
