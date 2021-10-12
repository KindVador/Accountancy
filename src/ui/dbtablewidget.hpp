#ifndef DBTABLEWIDGET_H
#define DBTABLEWIDGET_H

#include <QWidget>
#include <QAbstractItemModel>
#include <QString>

namespace Ui {
class DbTableWidget;
}

class DbTableWidget : public QWidget {
    Q_OBJECT

public:
    explicit DbTableWidget(QWidget *parent = nullptr);
    ~DbTableWidget();
    void setTableModel(QAbstractItemModel *model);
    void setTitle(QString title);

private:
    Ui::DbTableWidget *ui;
};

#endif // DBTABLEWIDGET_H
