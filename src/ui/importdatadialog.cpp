#include "importdatadialog.hpp"
#include "ui_importdatadialog.h"
#include "../model.hpp"

#include <QFileDialog>

ImportDataDialog::ImportDataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDataDialog)
{
    ui->setupUi(this);

    setWindowTitle("Import transactions to a account");

    // Populate owner ComboBox
    ui->ownerComboBox->setModel(Model::getInstance()->getOwnerModel());
    // Populate account ComboBox
    ui->accountComboBox->setModel(Model::getInstance()->getAccountFilter());

    connect(ui->addFileButton, &QPushButton::clicked, this, &ImportDataDialog::addFiles);
    connect(ui->removeFileButton, &QPushButton::clicked, this, &ImportDataDialog::removeSelectedFiles);
    connect(ui->ownerComboBox, &QComboBox::currentTextChanged, this, [this](const QString &text) { Model::getInstance()->setOwnerFilter(text);});
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
    if (fileDlg.exec() == DialogCode::Rejected)
        return;

    // add selected file to the QListWidget
    ui->filesListWidget->addItems(fileDlg.selectedFiles());
}

void ImportDataDialog::removeSelectedFiles()
{
    // remove selected items from the
    for (QListWidgetItem *item: ui->filesListWidget->selectedItems())
        ui->filesListWidget->takeItem(ui->filesListWidget->indexFromItem(item).row());
}

void ImportDataDialog::readFile(QFile &file)
{
    if (!file.open(QFile::ReadOnly | QFile::Text))
        return;

    int nbLinesToSkipped = 5;
    int nbLinesRead = 0;
    QTextStream inStream(&file);
    // Reads the data up to the end of file
    while (!inStream.atEnd()) {
        QString line = inStream.readLine();

        // skipped first line
        if (nbLinesRead < nbLinesToSkipped)
            continue;

        QStringList fields = line.split(";");
        // TODO

        ++nbLinesRead;
    }
    file.close();
}
