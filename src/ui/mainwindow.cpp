#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "contextualmenugenerator.hpp"
#include "importdatadialog.hpp"
#include "transactionswidget.hpp"
#include "../transactionmodel.hpp"
#include "../controller.hpp"

#include <QMessageBox>
#include <QString>
#include <QDebug>
#include <QMdiSubWindow>
#include <QAction>
#include <QFileDialog>

#include <vector>

constexpr const int ObjectRole = Qt::UserRole + 1;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Init Ui
    ui->ownersView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->actionSave->setEnabled(false);
    ui->actionSaveAs->setEnabled(false);

    // Connect Actions to Slots
    connect(ui->actionCredits, &QAction::triggered, this, &MainWindow::showCredits);
    connect(ui->ownersView, &QListView::clicked, this, [this](const QModelIndex &index) { emit selectedOwnerChanged(index); });
    connect(ui->accountsView, &QListView::clicked, this, [this](const QModelIndex &index) { emit selectedAccountChanged(index); });
    connect(ui->accountsView, &QListView::doubleClicked, this, &MainWindow::onAccountDoubleClicked);
    connect(ui->ownersView, &QListView::customContextMenuRequested, this, &MainWindow::contextualOwnerMenuRequested);
    connect(ui->actionMainDock, &QAction::triggered, this, &MainWindow::onActionMainDock);
    connect(ui->actionImport, &QAction::triggered, this, &MainWindow::onActionImport);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onOpenAction);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onSaveAction);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::onSaveAsAction);
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

    // connect Account model through AccountFilter model
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

void MainWindow::onAccountDoubleClicked(const QModelIndex &index)
{
    // get selected account
    auto *selectedAccount = _model->getAccountModel()->data(index, ObjectRole).value<Account *>();
    if (selectedAccount == nullptr)
        return;

    // delete previous central widget
    delete centralWidget();

    // replace central widget by a TransactionsWidget
    auto centralWidget = new TransactionsWidget();
    centralWidget->setTitle(selectedAccount->getDisplayedName());
    centralWidget->setModel(new TransactionModel(selectedAccount));
    setCentralWidget(centralWidget);
}

void MainWindow::onOpenAction()
{
    // show dialog window to select file
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Open File",
                                                    QDir::homePath(),
                                                    "Accountancy files (*.acty)");

    // update current file variable
    Controller *controller = Controller::getInstance();
    if (controller->loadFile(fileName)) {
        ui->actionSave->setEnabled(true);
        ui->actionSaveAs->setEnabled(true);
    }
}

void MainWindow::onSaveAction()
{
    Controller *controller = Controller::getInstance();
    controller->saveToFile(controller->getCurrentFilePath());
}

void MainWindow::onSaveAsAction()
{
    // show dialog window to select file
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Save File As",
                                                    QDir::homePath(),
                                                    "Accountancy files (*.acty)");

    // update current file variable
    Controller *controller = Controller::getInstance();
    controller->saveToFile(fileName);
}
