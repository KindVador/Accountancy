#ifndef ACCOUNTANCY_ADDACCOUNTDIALOG_HPP
#define ACCOUNTANCY_ADDACCOUNTDIALOG_HPP

#include "../core/account.hpp"
#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui
{
    class AddAccountDialog;
}
QT_END_NAMESPACE

class AddAccountDialog : public QDialog
{

    Q_OBJECT

public:
    explicit AddAccountDialog(QWidget* parent = nullptr);
    ~AddAccountDialog() override;

private:
    [[nodiscard]] QList<const Owner*> getSelectedOwners() const;

public slots:
    void accept() override;
    void reject() override;

private:
    Ui::AddAccountDialog* ui;
};

#endif//ACCOUNTANCY_ADDACCOUNTDIALOG_HPP
