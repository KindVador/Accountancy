#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "contextualmenugenerator.hpp"

#include <QMessageBox>
#include <QString>
#include <QDebug>
#include <QMdiSubWindow>
#include <QLabel>
#include <QAction>
#include <vector>
#include <QFileDialog>

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Init Ui
    ui->actionDisconnect->setEnabled(false);
    ui->ownersView->setContextMenuPolicy(Qt::CustomContextMenu);

    // Connect Actions to Slots
    connect(ui->actionCredits, &QAction::triggered, this, &MainWindow::showCredits);
    connect(ui->ownersView, &QListView::clicked, this, [this](const QModelIndex &index) { emit selectedOwnerChanged(index); });
    connect(ui->accountsView, &QListView::clicked, this, [this](const QModelIndex &index) { emit selectedAccountChanged(index); });
    connect(ui->ownersView, &QListView::customContextMenuRequested, this, &MainWindow::contextualOwnerMenuRequested);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::showCredits()
{
    QString text(tr("Icônes faites par Pixel perfect de www.flaticon.com"));  // In english, Icons made by Pixel perfect from www.flaticon.com
    QMessageBox::information(this, "Credits", text);
}

void MainWindow::on_actionImport_triggered()
{
    // TODO create a window to import data into DataBase
    qDebug() << "MainWindow::on_actionImport_triggered()" << Qt::endl;
    QFileDialog fileDlg(this);
    fileDlg.setFileMode(QFileDialog::ExistingFiles);
    fileDlg.setViewMode(QFileDialog::Detail);

    QStringList fileNames;
    if (fileDlg.exec()) {
        fileNames = fileDlg.selectedFiles();
    }

    // printing all selected file
    QStringList::const_iterator constIterator;
    for (constIterator = fileNames.constBegin(); constIterator != fileNames.constEnd(); ++constIterator) {
        qDebug() << *constIterator << Qt::endl;
        QFile dataFile(*constIterator);
        if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text))
                return;

            QTextStream in(&dataFile);
            while (!in.atEnd()) {
                QString line = in.readLine();
                qDebug() << line;
            }
    }
}

void MainWindow::onOwnerModelUpdate()
{
}

Model *MainWindow::getModel() const
{
    return _model;
}

void MainWindow::setModel(Model *model)
{
    qWarning() << "MainWindow::setModel";
    _model = model;

    // connect Owner model
    ui->ownersView->setModel(_model->getOwnerModel());

    // connect Account model through AccountFilder model
    ui->accountsView->setModel(_model->getAccountFilter());
}

void MainWindow::contextualOwnerMenuRequested(const QPoint &pos)
{
    std::unique_ptr<QMenu> contextMenu;
    contextMenu.reset(ContextualMenuGenerator::ownerSectionMenu(this));

    if (contextMenu != nullptr)
        contextMenu->exec(QCursor::pos());
}
