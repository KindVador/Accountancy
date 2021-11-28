#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "contextualmenugenerator.hpp"
#include "importdatadialog.hpp"

#include <QMessageBox>
#include <QString>
#include <QDebug>
#include <QMdiSubWindow>
#include <QAction>
#include <vector>

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Init Ui
    ui->ownersView->setContextMenuPolicy(Qt::CustomContextMenu);

    // Connect Actions to Slots
    connect(ui->actionCredits, &QAction::triggered, this, &MainWindow::showCredits);
    connect(ui->ownersView, &QListView::clicked, this, [this](const QModelIndex &index) { emit selectedOwnerChanged(index); });
    connect(ui->accountsView, &QListView::clicked, this, [this](const QModelIndex &index) { emit selectedAccountChanged(index); });
    connect(ui->ownersView, &QListView::customContextMenuRequested, this, &MainWindow::contextualOwnerMenuRequested);
    connect(ui->actionMainDock, &QAction::triggered, this, &MainWindow::onActionMainDock);
    connect(ui->actionImport, &QAction::triggered, this, &MainWindow::onActionImport);
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

void MainWindow::onActionImport()
{
    ImportDataDialog dialog = ImportDataDialog();
    dialog.exec();
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

void MainWindow::onActionMainDock(bool checked)
{
    if (!checked)
        ui->mainDockWidget->close();
    else
        ui->mainDockWidget->show();
}
