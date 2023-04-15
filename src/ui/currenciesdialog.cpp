#include "currenciesdialog.hpp"
#include "controller/controller.hpp"
#include "createcurrencydialog.hpp"
#include "ui_currenciesdialog.h"

CurrenciesDialog::CurrenciesDialog(QWidget* parent, CurrencyModel* model) : QDialog(parent), ui(new Ui::CurrenciesDialog), _model(model)
{
    ui->setupUi(this);

    // init TableView model
    ui->tableView->setModel(_model);
    if (_model != nullptr)
        qWarning() << "    ==> Init of CurrenciesDialog with model: " << _model->getName();
    else
        qWarning() << "    ==> Init of CurrenciesDialog with NULL ptr";

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
    auto dlg = CreateCurrencyDialog(this);
    dlg.exec();
}

void CurrenciesDialog::onRemoveCurrencyAction() const
{
    QList<QModelIndex> selectedIndexes = ui->tableView->selectionModel()->selectedIndexes();
    for (QModelIndex selectedIndex: selectedIndexes) {
        QUuid uid = _model->data(selectedIndex, Qt::UserRole).toUuid();
        Controller::instance()->removeCurrency(uid);
    }
}
