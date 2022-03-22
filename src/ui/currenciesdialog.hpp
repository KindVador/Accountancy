#ifndef ACCOUNTANCY_CURRENCIESDIALOG_HPP
#define ACCOUNTANCY_CURRENCIESDIALOG_HPP

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class CurrenciesDialog; }
QT_END_NAMESPACE

class CurrenciesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CurrenciesDialog(QWidget *parent = nullptr);
    ~CurrenciesDialog() override;

public slots:
    void accept() override;
    void reject() override;

private:
    Ui::CurrenciesDialog *ui;
};

#endif //ACCOUNTANCY_CURRENCIESDIALOG_HPP
