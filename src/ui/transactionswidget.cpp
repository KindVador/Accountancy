// You may need to build the project (run Qt uic code generator) to get "ui_TransactionsWidget.h" resolved
#include "transactionswidget.hpp"
#include "ui_transactionswidget.h"

#include <QMenu>

TransactionsWidget::TransactionsWidget(QWidget* parent) : QWidget(parent), ui(new Ui::TransactionsWidget)
{
    ui->setupUi(this);

    // change Qt::ItemDataRole used to sort table
    _proxyModel->setSortRole(Qt::UserRole);

    // init tableView
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->sortByColumn(0, Qt::SortOrder::DescendingOrder);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, &QTableView::customContextMenuRequested, this, &TransactionsWidget::customMenuRequested);
}

TransactionsWidget::~TransactionsWidget()
{
    delete ui;
}


void TransactionsWidget::setModel(TransactionModel* model)
{
    if (ui == nullptr)
        return;

    _proxyModel->setSourceModel(model);
    ui->tableView->setModel(_proxyModel.get());
    ui->tableView->sortByColumn(0, Qt::SortOrder::DescendingOrder);
}

void TransactionsWidget::setTitle(const QString& text)
{
    if (ui == nullptr)
        return;

    ui->titleLabel->setText(text);
}

void TransactionsWidget::customMenuRequested(QPoint pos)
{
    static const QIcon deleteIcon = QPixmap(":/icns/black/resources/icons/trash.svg");
    QModelIndex index = ui->tableView->indexAt(pos);

    auto* menu = new QMenu(this);
    menu->addAction(deleteIcon, "Delete Transaction(s)", this, []() { ; });
    menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}