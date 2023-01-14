#ifndef ACCOUNTANCY_INSTITUTIONSDIALOG_HPP
#define ACCOUNTANCY_INSTITUTIONSDIALOG_HPP

#include "../core/financialinstitutionmodel.hpp"
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class InstitutionsDialog;
}
QT_END_NAMESPACE

class InstitutionsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InstitutionsDialog(QWidget* parent, FinancialInstitutionModel* model);
    ~InstitutionsDialog() override;

public slots:
    void accept() override;
    void reject() override;

private slots:
    void onAddInstitutionAction();
    void onRemoveInstitutionAction() const;

private:
    Ui::InstitutionsDialog* ui;
    FinancialInstitutionModel* _model = nullptr;
};

#endif//ACCOUNTANCY_INSTITUTIONSDIALOG_HPP
