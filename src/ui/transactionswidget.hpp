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

class TransactionsWidget : public QWidget
{

    Q_OBJECT

public:
    explicit TransactionsWidget(Account* account, QWidget* parent = nullptr);
    ~TransactionsWidget() override;

    void setTitle(const QString& text);

public slots:
    void customMenuRequested(QPoint pos);

private:
    Ui::TransactionsWidget* ui;
    std::unique_ptr<QSortFilterProxyModel> _proxyModel = std::make_unique<QSortFilterProxyModel>();
    std::unique_ptr<TransactionModel> _transaction_model = std::make_unique<TransactionModel>();
};

#endif//ACCOUNTANCY_TRANSACTIONSWIDGET_HPP
