// You may need to build the project (run Qt uic code generator) to get "ui_TransactionsWidget.h" resolved

#include "transactionswidget.hpp"
#include "ui_TransactionsWidget.h"


TransactionsWidget::TransactionsWidget(QWidget *parent) :
        QWidget(parent), ui(new Ui::TransactionsWidget)
{
    ui->setupUi(this);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

TransactionsWidget::~TransactionsWidget()
{
    delete ui;
}


void TransactionsWidget::setModel(TransactionModel *model)
{
    if (ui == nullptr)
        return;

    ui->tableView->setModel(model);
}

void TransactionsWidget::setTitle(QString text)
{
    if (ui == nullptr)
        return;

    ui->titleLabel->setText(text);
}
