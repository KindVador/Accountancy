#include "mainwindow.hpp"
#include "addaccountdialog.hpp"
#include "addownerdialog.hpp"
#include "categoriestreedialog.hpp"
#include "contextualmenugenerator.hpp"
#include "controller/controller.hpp"
#include "currenciesdialog.hpp"
#include "importdatadialog.hpp"
#include "institutionsdialog.hpp"
#include "transactionswidget.hpp"
#include "ui_mainwindow.h"

#include <QAction>
#include <QDebug>
#include <QFileDialog>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QString>

constexpr const int ObjectRole = Qt::UserRole + 1;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow())
{
    ui->setupUi(this);

    // File Menu
    ui->actionSave->setEnabled(false);
    ui->actionSaveAs->setEnabled(false);
    connect(ui->actionCreate, &QAction::triggered, this, &MainWindow::onCreateAction);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onOpenAction);
    connect(ui->actionImport, &QAction::triggered, this, &MainWindow::onActionImport);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onSaveAction);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::onSaveAsAction);
    connect(ui->actionClose, &QAction::triggered, this, &MainWindow::onCloseAction);

    // Edit Menu
    connect(ui->actionCurrencies, &QAction::triggered, this, &MainWindow::onCurrenciesAction);
    connect(ui->actionInstitutions, &QAction::triggered, this, &MainWindow::onInstitutionsAction);
    connect(ui->actionCategories, &QAction::triggered, this, &MainWindow::onCategoriesAction);

    // View Menu
    connect(ui->actionMainDock, &QAction::triggered, this, &MainWindow::onActionMainDock);

    // Help Menu
    connect(ui->actionCredits, &QAction::triggered, this, &MainWindow::showCredits);

    // Main Dock Widget
    ui->ownersView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->ownersView, &QListView::clicked, this, [this](const QModelIndex& index) { emit selectedOwnerChanged(index); });
    connect(ui->accountsView, &QListView::clicked, this, [this](const QModelIndex& index) { emit selectedAccountChanged(index); });
    connect(ui->accountsView, &QListView::doubleClicked, this, &MainWindow::onAccountDoubleClicked);
    connect(ui->ownersView, &QListView::customContextMenuRequested, this, &MainWindow::contextualOwnerMenuRequested);
    connect(ui->addOwnerButton, &QPushButton::clicked, this, &MainWindow::onAddOwnerAction);
    connect(ui->removeOwnerButton, &QPushButton::clicked, this, &MainWindow::onRemoveOwnerAction);
    connect(ui->addAccountButton, &QPushButton::clicked, this, &MainWindow::onAddAccountAction);
    connect(ui->removeAccountButton, &QPushButton::clicked, this, &MainWindow::onRemoveAccountAction);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showCredits()
{
    QString text = QCoreApplication::applicationVersion().prepend("Version: ");
    text.append(tr("\nIcônes faites par Pixel perfect de www.flaticon.com"));// In english, Icons made by Pixel perfect from www.flaticon.com
    QMessageBox::information(this, "Credits", text);
}

void MainWindow::onActionImport()
{
    auto dialog = ImportDataDialog(this);
    dialog.exec();
}

Model* MainWindow::getModel() const
{
    return _model.get();
}

void MainWindow::setModel(Model* model)
{
    qWarning() << "MainWindow::setModel";
    _model.reset(model);

    // connect Owner model
    if (auto ownerModel = _model->getModel<OwnerModel>("OwnerModel"); model != nullptr) {
        ui->ownersView->setModel(ownerModel);
    } else {
        qCritical() << "Invalid Owner model";
    }

    // connect Account model through AccountFilter model
    ui->accountsView->setModel(_model->getAccountFilter());
}

void MainWindow::contextualOwnerMenuRequested(const QPoint& pos) const
{
    Q_UNUSED(pos)
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

void MainWindow::onAccountDoubleClicked(const QModelIndex& index)
{
    // get selected account
    Account* selectedAccount = nullptr;
    if (auto model = _model->getModel<AccountModel>("AccountModel"); model != nullptr) {
        selectedAccount = model->data(index, ObjectRole).value<Account*>();
    } else {
        qWarning() << "Invalid AccountModel";
    }
    if (selectedAccount == nullptr)
        return;

    // delete previous central widget
    delete centralWidget();

    // replace central widget by a TransactionsWidget
    auto centralWidget = new TransactionsWidget(selectedAccount, this);
    centralWidget->setTitle(selectedAccount->getDisplayedName());
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
    Controller* controller = Controller::instance();
    if (controller->loadFile(fileName))
        updateEditionInterface(true);
}

void MainWindow::onSaveAction() const
{
    Controller* controller = Controller::instance();
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
    Controller* controller = Controller::instance();
    controller->saveToFile(fileName);
}

void MainWindow::onCreateAction()
{
    // show dialog window to select file
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Create new file",
                                                    QDir::homePath(),
                                                    "Accountancy files (*.acty)");

    // update current file variable
    Controller* controller = Controller::instance();
    if (controller->createNewFile(fileName))
        updateEditionInterface(true);
}

void MainWindow::onAddOwnerAction()
{
    auto dialog = AddOwnerDialog(this);
    dialog.exec();
}

void MainWindow::updateEditionInterface(bool enable)
{
    ui->actionImport->setEnabled(enable);
    ui->actionSave->setEnabled(enable);
    ui->actionSaveAs->setEnabled(enable);
    ui->addOwnerButton->setEnabled(enable);
    ui->removeOwnerButton->setEnabled(enable);
    ui->addAccountButton->setEnabled(enable);
    ui->removeAccountButton->setEnabled(enable);
}

void MainWindow::onRemoveOwnerAction()
{
    if (auto ownerModel = _model->getModel<OwnerModel>("OwnerModel"); ownerModel != nullptr) {
        QList<QModelIndex> selIndexes = ui->ownersView->selectionModel()->selectedIndexes();
        for (const QModelIndex& selIndex: qAsConst(selIndexes))
            ownerModel->removeOwner(selIndex);
    } else {
        qWarning() << "Invalid OwnerModel";
    }
}

void MainWindow::onAddAccountAction()
{
    auto dialog = AddAccountDialog(this);
    dialog.exec();
}

void MainWindow::onRemoveAccountAction()
{
    if (auto model = _model->getModel<AccountModel>("AccountModel"); model != nullptr) {
        QList<QModelIndex> selIndexes = ui->accountsView->selectionModel()->selectedIndexes();
        for (const QModelIndex& selIndex: qAsConst(selIndexes))
            model->removeAccount(selIndex);
    } else {
        qWarning() << "Invalid AccountModel";
    }
}

void MainWindow::onCurrenciesAction()
{
    auto dlg = CurrenciesDialog(this, _model->getModel<CurrencyModel>("CurrencyModel"));
    dlg.exec();
}

void MainWindow::onInstitutionsAction()
{
    auto dlg = InstitutionsDialog(this, _model->getModel<FinancialInstitutionModel>("FinancialInstitutionModel"));
    dlg.exec();
}

void MainWindow::onCategoriesAction()
{
    auto dlg = CategoriesTreeDialog(this, _model->getModel<CategoryModel>("CategoryModel"));
    dlg.exec();
}

void MainWindow::onCloseAction()
{
    if (Controller* controller = Controller::instance())
        controller->saveSettings();

    close();
}
