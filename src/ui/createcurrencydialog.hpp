#ifndef ACCOUNTANCY_CREATECURRENCYDIALOG_HPP
#define ACCOUNTANCY_CREATECURRENCYDIALOG_HPP

#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class CreateCurrencyDialog; }
QT_END_NAMESPACE

class CreateCurrencyDialog : public QDialog
{
Q_OBJECT

public:
    explicit CreateCurrencyDialog(QWidget *parent = nullptr);

    ~CreateCurrencyDialog() override;

public slots:
    void accept() override;
    void reject() override;

private:
    Ui::CreateCurrencyDialog *ui;
};

#endif //ACCOUNTANCY_CREATECURRENCYDIALOG_HPP
