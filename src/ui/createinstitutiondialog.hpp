//
// Created by Florian Contival on 27/03/2022.
//

#ifndef ACCOUNTANCY_CREATEINSTITUTIONDIALOG_HPP
#define ACCOUNTANCY_CREATEINSTITUTIONDIALOG_HPP

#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui
{
    class CreateInstitutionDialog;
}
QT_END_NAMESPACE

class CreateInstitutionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateInstitutionDialog(QWidget* parent = nullptr);

    ~CreateInstitutionDialog() override;

public slots:
    void accept() override;
    void reject() override;

private:
    Ui::CreateInstitutionDialog* ui;
};


#endif//ACCOUNTANCY_CREATEINSTITUTIONDIALOG_HPP
