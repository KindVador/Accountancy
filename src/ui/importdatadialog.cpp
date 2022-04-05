#include "importdatadialog.hpp"
#include "ui_importdatadialog.h"
#include "../core/model.hpp"

#include <QFileDialog>

constexpr const int ObjectRole = Qt::UserRole + 1;

ImportDataDialog::ImportDataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDataDialog)
{
    ui->setupUi(this);

    setWindowTitle("Import transactions to an account");

    // Populate owner ComboBox
    ui->ownerComboBox->setModel(Model::instance()->getOwnerModel());
    // Populate account ComboBox
    ui->accountComboBox->setModel(Model::instance()->getAccountFilter());

    connect(ui->addFileButton, &QPushButton::clicked, this, &ImportDataDialog::addFiles);
    connect(ui->removeFileButton, &QPushButton::clicked, this, &ImportDataDialog::removeSelectedFiles);
    connect(ui->ownerComboBox, &QComboBox::currentTextChanged, this, [this](const QString &text) { Model::instance()->setOwnerFilter(text);});
}

ImportDataDialog::~ImportDataDialog()
{
    delete ui;
}

void ImportDataDialog::addFiles()
{
    // show dialog to select files
    QFileDialog fileDlg(this);
    fileDlg.setFileMode(QFileDialog::ExistingFiles);
    fileDlg.setViewMode(QFileDialog::Detail);

    // case if user close the dialog without selecting any file
    if (fileDlg.exec() == QFileDialog::Reject)
        return;

    // add selected file to the QListWidget
    ui->filesListWidget->addItems(fileDlg.selectedFiles());
}

void ImportDataDialog::removeSelectedFiles()
{
    // remove selected items from the list
    for (QListWidgetItem *item: ui->filesListWidget->selectedItems())
        ui->filesListWidget->takeItem(ui->filesListWidget->indexFromItem(item).row());
}

void ImportDataDialog::accept()
{
    // get selected Account
    auto *account = ui->accountComboBox->currentData(ObjectRole).value<Account *>();
    if (account == nullptr)
        return;

    // get Financial Institution from selected Account
    auto financialInstitution = account->getInstitution();

    // read transactions from files
    QList<Transaction *> transactions;
    for (int i = 0; i < ui->filesListWidget->count(); ++i) {
        const QListWidgetItem *item = ui->filesListWidget->item(i);
        auto dataFile = QFile(item->text());
        transactions.append(financialInstitution->readTransactionsFromFile(dataFile));
    }

    // add transactions to the selected count
    for (Transaction *t : transactions)
        account->addTransaction(t);

    QDialog::accept();
}

void ImportDataDialog::reject()
{
    QDialog::reject();
}
