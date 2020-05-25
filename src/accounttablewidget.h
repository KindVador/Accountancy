#ifndef ACCOUNTTABLEWIDGET_H
#define ACCOUNTTABLEWIDGET_H

#include <QWidget>
#include <QAbstractItemModel>

namespace Ui {
class AccountTableWidget;
}

class AccountTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AccountTableWidget(QWidget *parent = nullptr);
    ~AccountTableWidget();
    void setTableModel(QAbstractItemModel *model);

private:
    Ui::AccountTableWidget *ui;
};

#endif // ACCOUNTTABLEWIDGET_H
