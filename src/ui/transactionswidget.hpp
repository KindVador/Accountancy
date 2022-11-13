#ifndef ACCOUNTANCY_TRANSACTIONSWIDGET_HPP
#define ACCOUNTANCY_TRANSACTIONSWIDGET_HPP

#include "../core/transactionmodel.hpp"
#include <QSortFilterProxyModel>
#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui
{
    class TransactionsWidget;
}
QT_END_NAMESPACE

namespace Ui
{
    class TransactionsWidget;
}

class TransactionsWidget : public QWidget
{

    Q_OBJECT

public:
    explicit TransactionsWidget(QWidget* parent = nullptr);
    ~TransactionsWidget() override;

    void setModel(TransactionModel* model);
    void setTitle(const QString& text);

public slots:
    void customMenuRequested(QPoint pos);

private:
    Ui::TransactionsWidget* ui;
    std::unique_ptr<QSortFilterProxyModel> _proxyModel = std::make_unique<QSortFilterProxyModel>();
};


#endif//ACCOUNTANCY_TRANSACTIONSWIDGET_HPP
