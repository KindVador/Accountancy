// You may need to build the project (run Qt uic code generator) to get "ui_TransactionsWidget.h" resolved
#include "transactionswidget.hpp"
#include "../core/model.hpp"
#include "ui_transactionswidget.h"

#include <algorithm>

#include <QMenu>
#include <QUuid>
#include <QVector>

constexpr const int ObjectRole = Qt::UserRole + 1;
constexpr const int UIDRole = Qt::UserRole + 2;

TransactionsWidget::TransactionsWidget(Account* account, QWidget* parent) : QWidget(parent), ui(new Ui::TransactionsWidget)
{
    ui->setupUi(this);

    // change Qt::ItemDataRole used to sort table
    _proxyModel->setSortRole(Qt::UserRole);

    // init model
    _transaction_model->setAccount(account);
    _proxyModel->setSourceModel(_transaction_model.get());
    ui->tableView->setModel(_proxyModel.get());
    ui->tableView->sortByColumn(0, Qt::SortOrder::DescendingOrder);

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

void TransactionsWidget::setTitle(const QString& text)
{
    if (ui == nullptr)
        return;

    ui->titleLabel->setText(text);
}

void TransactionsWidget::customMenuRequested(QPoint pos)
{
    // retrieve model to update
    auto model = dynamic_cast<TransactionModel*>(_proxyModel->sourceModel());
    if (model == nullptr)
        return;

    static const QIcon deleteIcon = QPixmap(":/icns/black/resources/icons/trash.svg");

    QList<QModelIndex> selectedIndexes = ui->tableView->selectionModel()->selectedIndexes();
    std::transform(selectedIndexes.cbegin(), selectedIndexes.cend(), selectedIndexes.begin(), [this](const QModelIndex& index) {
        return _proxyModel->mapToSource(index);
    });

    // Sort indexes to keep valid indexes during deletion (we start by the higher index to avoid shifting indexes in transactions list stored in Account)
    std::sort(selectedIndexes.begin(), selectedIndexes.end(), [](const QModelIndex& a, const QModelIndex& b) {
        return a.row() > b.row();
    });

    auto* menu = new QMenu(this);
    menu->addAction(deleteIcon, "Delete Transaction(s)", this, [model, selectedIndexes]() {
        for (const QModelIndex& index: selectedIndexes)
            model->removeTransaction(model->data(index, UIDRole).toUuid());
    });
    menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}