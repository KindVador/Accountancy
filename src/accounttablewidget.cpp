#include "accounttablewidget.h"
#include "ui_accounttablewidget.h"
#include <QDebug>

AccountTableWidget::AccountTableWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AccountTableWidget)
{
    ui->setupUi(this);

    // Update of Title with the correct Account's name.
    ui->labelTitle->setText("Account #1");
    qDebug() << "Fin de la création de AccountTableWidget";
}

AccountTableWidget::~AccountTableWidget()
{
    delete ui;
    qDebug() << "Fin de la destruction de AccountTableWidget";
}

void AccountTableWidget::setTableModel(QAbstractItemModel *model)
{
    ui->tableView->setModel(model);
    qDebug() << "setTableModel";
}

void AccountTableWidget::setTitle(QString title)
{
    ui->labelTitle->setText(title);
}
