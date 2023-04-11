#ifndef ACCOUNTANCY_TRANSACTIONSWIDGET_HPP
#define ACCOUNTANCY_TRANSACTIONSWIDGET_HPP

#include "comboboxitemdelegate.hpp"
#include "model/transactionmodel.hpp"

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

private slots:
    void onDoubleClicked(const QModelIndex& index);

private:
    Ui::TransactionsWidget* ui;
    std::unique_ptr<QSortFilterProxyModel> _proxyModel = std::make_unique<QSortFilterProxyModel>();
    std::unique_ptr<TransactionModel> _transaction_model = std::make_unique<TransactionModel>("LocalTransactionModel");
    std::unique_ptr<ComboBoxItemDelegate> _category_item_delegate = std::make_unique<ComboBoxItemDelegate>();

    bool selectCategory(Transaction* transaction);
};

#endif//ACCOUNTANCY_TRANSACTIONSWIDGET_HPP
