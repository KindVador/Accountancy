#ifndef ACCOUNTANCY_TRANSACTIONSWIDGET_HPP
#define ACCOUNTANCY_TRANSACTIONSWIDGET_HPP

#include <QWidget>
#include "../core/transactionmodel.hpp"


QT_BEGIN_NAMESPACE
namespace Ui { class TransactionsWidget; }
QT_END_NAMESPACE

namespace Ui {
    class TransactionsWidget;
}

class TransactionsWidget : public QWidget
{

    Q_OBJECT

public:
    explicit TransactionsWidget(QWidget *parent = nullptr);
    ~TransactionsWidget() override;

    void setModel(TransactionModel *model);
    void setTitle(QString text);

private:
    Ui::TransactionsWidget *ui;
};


#endif //ACCOUNTANCY_TRANSACTIONSWIDGET_HPP
