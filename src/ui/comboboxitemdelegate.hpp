#ifndef ACCOUNTANCY_COMBOBOXITEMDELEGATE_HPP
#define ACCOUNTANCY_COMBOBOXITEMDELEGATE_HPP

#include <QStyledItemDelegate>

// https://wiki.qt.io/Combo_Boxes_in_Item_Views


class ComboBoxItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ComboBoxItemDelegate(QObject* parent = nullptr);
    ~ComboBoxItemDelegate();

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
};


#endif//ACCOUNTANCY_COMBOBOXITEMDELEGATE_HPP
