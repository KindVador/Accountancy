#ifndef ACCOUNTANCY_CURRENCIESDIALOG_HPP
#define ACCOUNTANCY_CURRENCIESDIALOG_HPP

#include <QDialog>

#include "model/currencymodel.hpp"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class CurrenciesDialog;
}
QT_END_NAMESPACE

class CurrenciesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CurrenciesDialog(QWidget* parent, CurrencyModel* model);
    ~CurrenciesDialog() override;

public slots:
    void accept() override;
    void reject() override;

private slots:
    void onAddCurrencyAction();
    void onRemoveCurrencyAction() const;

private:
    Ui::CurrenciesDialog* ui;
    CurrencyModel* _model = nullptr;
};

#endif//ACCOUNTANCY_CURRENCIESDIALOG_HPP
