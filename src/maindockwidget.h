#ifndef MAINDOCKWIDGET_H
#define MAINDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class MainDockWidget;
}

class MainDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit MainDockWidget(QWidget *parent = nullptr);
    ~MainDockWidget();

private:
    Ui::MainDockWidget *ui;
};

#endif // MAINDOCKWIDGET_H
