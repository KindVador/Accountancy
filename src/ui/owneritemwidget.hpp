#ifndef OWNERITEMWIDGET_HPP
#define OWNERITEMWIDGET_HPP

#include <QWidget>

namespace Ui
{
    class OwnerItemWidget;
}

class OwnerItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OwnerItemWidget(QWidget* parent = nullptr);
    ~OwnerItemWidget();

private:
    Ui::OwnerItemWidget* ui;
};

#endif// OWNERITEMWIDGET_HPP
