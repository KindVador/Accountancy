#include "dbtablewidget.hpp"
#include "ui_dbtablewidget.h"
#include <QDebug>

DbTableWidget::DbTableWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DbTableWidget) {
    ui->setupUi(this);
    qDebug() << "Fin de la création de AccountTableWidget";
}

DbTableWidget::~DbTableWidget() {
    delete ui;
    qDebug() << "Fin de la destruction de AccountTableWidget";
}

void DbTableWidget::setTableModel(QAbstractItemModel *model) {
    ui->tableView->setModel(model);
    qDebug() << "setTableModel";
}

void DbTableWidget::setTitle(QString title) {
    ui->labelTitle->setText(title);
}
