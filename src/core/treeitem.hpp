#ifndef ACCOUNTANCY_TREEITEM_HPP
#define ACCOUNTANCY_TREEITEM_HPP

#include <QList>
#include <QVariant>

class TreeItem
{
public:
    explicit TreeItem(const QList<QVariant>& data, TreeItem* parentItem = nullptr);
    ~TreeItem();

    void appendChild(TreeItem* child);

    TreeItem* child(int row);
    [[nodiscard]] int childCount() const;
    [[nodiscard]] int columnCount() const;
    [[nodiscard]] QVariant data(int column) const;
    [[nodiscard]] int row() const;
    TreeItem* parentItem();

private:
    QList<TreeItem*> _childItems;
    QList<QVariant> _itemData;
    TreeItem* _parentItem;
};

#endif//ACCOUNTANCY_TREEITEM_HPP
