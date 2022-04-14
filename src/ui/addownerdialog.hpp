#ifndef ACCOUNTANCY_ADDOWNERDIALOG_HPP
#define ACCOUNTANCY_ADDOWNERDIALOG_HPP

#include <QDialog>
#include "../core/owner.hpp"


QT_BEGIN_NAMESPACE
namespace Ui { class AddOwnerDialog; }
QT_END_NAMESPACE

class AddOwnerDialog : public QDialog
{
Q_OBJECT

public:
    explicit AddOwnerDialog(QWidget *parent = nullptr);
    ~AddOwnerDialog() override;

public slots:
    void accept() override;
    void reject() override;

private:
    Ui::AddOwnerDialog *ui;
};


#endif //ACCOUNTANCY_ADDOWNERDIALOG_HPP
