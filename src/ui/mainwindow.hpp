#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class Model;

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    [[nodiscard]] Model* getModel() const;
    void setModel(Model* model);

public slots:
    void updateEditionInterface(bool enable);

signals:
    void selectedOwnerChanged(const QModelIndex& ownerIndex);
    void selectedAccountChanged(const QModelIndex& accountIndex);

private slots:
    void onCreateAction();
    void onOpenAction();
    void onSaveAction();
    void onSaveAsAction();
    void showCredits();
    void onActionImport();
    void contextualOwnerMenuRequested(const QPoint& pos);
    void onActionMainDock(bool checked);
    void onAccountDoubleClicked(const QModelIndex& index);
    void onAddOwnerAction();
    void onRemoveOwnerAction();
    void onAddAccountAction();
    void onRemoveAccountAction();
    void onCurrenciesAction();
    void onInstitutionsAction();

private:
    Ui::MainWindow* ui = nullptr;
    Model* _model = nullptr;
};

#endif// MAINWINDOW_H
