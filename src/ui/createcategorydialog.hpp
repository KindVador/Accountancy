#ifndef ACCOUNTANCY_CREATECATEGORYDIALOG_HPP
#define ACCOUNTANCY_CREATECATEGORYDIALOG_HPP

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class CreateCategoryDialog;
}
QT_END_NAMESPACE

class CreateCategoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateCategoryDialog(QWidget* parent = nullptr);

    ~CreateCategoryDialog() override;

public slots:
    void accept() override;
    void reject() override;

private:
    Ui::CreateCategoryDialog* ui;
};


#endif//ACCOUNTANCY_CREATECATEGORYDIALOG_HPP
