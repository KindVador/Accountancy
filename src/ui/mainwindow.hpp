#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include "../model.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Model *getModel() const;
    void setModel(Model *model);

public slots:
    void  onOwnerModelUpdate();

signals:
    void selectedOwnerChanged(const QModelIndex &ownerIndex);
    void selectedAccountChanged(const QModelIndex &accountIndex);

private slots:
    void showCredits();
    void on_actionImport_triggered();

private:
    Ui::MainWindow *ui = nullptr;
    Model *_model = nullptr;
};

#endif // MAINWINDOW_H
