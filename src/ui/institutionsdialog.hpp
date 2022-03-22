#ifndef ACCOUNTANCY_INSTITUTIONSDIALOG_HPP
#define ACCOUNTANCY_INSTITUTIONSDIALOG_HPP

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class InstitutionsDialog; }
QT_END_NAMESPACE

class InstitutionsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InstitutionsDialog(QWidget *parent = nullptr);
    ~InstitutionsDialog() override;

public slots:
    void accept() override;
    void reject() override;

private:
    Ui::InstitutionsDialog *ui;
};

#endif //ACCOUNTANCY_INSTITUTIONSDIALOG_HPP
