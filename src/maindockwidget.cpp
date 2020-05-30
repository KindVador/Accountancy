#include "maindockwidget.h"
#include "ui_maindockwidget.h"

MainDockWidget::MainDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::MainDockWidget)
{
    ui->setupUi(this);
}

MainDockWidget::~MainDockWidget()
{
    delete ui;
}
