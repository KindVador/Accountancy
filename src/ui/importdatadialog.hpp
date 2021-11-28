#ifndef IMPORTDATADIALOG_HPP
#define IMPORTDATADIALOG_HPP

#include <QDialog>
#include <QFile>

namespace Ui {
class ImportDataDialog;
}

class ImportDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportDataDialog(QWidget *parent = nullptr);
    ~ImportDataDialog() override;

private slots:
    void addFiles();
    void removeSelectedFiles();
    void readFile(QFile &file);

private:
    Ui::ImportDataDialog *ui;
};

#endif // IMPORTDATADIALOG_HPP
