#include "importdatadialog.hpp"
#include "ui_importdatadialog.h"
#include "../model.hpp"

#include <QFileDialog>

ImportDataDialog::ImportDataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDataDialog)
{
    ui->setupUi(this);

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

    // Create a thread to retrieve data from a file
    QTextStream in(&file);
    //Reads the data up to the end of file
    while (!in.atEnd()) {
        QString line = in.readLine();
        qDebug() << line;
        // Adding to the model in line with the elements
//        QList<QStandardItem *> standardItemsList;
//        // consider that the line separated by semicolons into columns
//        for (QString item : line.split(";")) {
//            standardItemsList.append(new QStandardItem(item));
//        }
//        csvModel->insertRow(csvModel->rowCount(), standardItemsList);
    }
    file.close();
}
