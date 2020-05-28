#ifndef ACCOUNTTABLEWIDGET_H
#define ACCOUNTTABLEWIDGET_H

#include <QWidget>
#include <QAbstractItemModel>
#include <QString>

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
    void setTitle(QString title);

private:
    Ui::AccountTableWidget *ui;
};

#endif // ACCOUNTTABLEWIDGET_H
