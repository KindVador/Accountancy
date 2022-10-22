#include "owneritemwidget.hpp"
#include "ui_owneritemwidget.h"

OwnerItemWidget::OwnerItemWidget(QWidget* parent) : QWidget(parent),
                                                    ui(new Ui::OwnerItemWidget)
{
    ui->setupUi(this);
}

OwnerItemWidget::~OwnerItemWidget()
{
    delete ui;
}
