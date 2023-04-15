#include "treeitem.hpp"

TreeItem::TreeItem(const QList<QVariant>& data, TreeItem* parentItem)
    : _itemData(data), _parentItem(parentItem)
{
}

TreeItem::~TreeItem()
{
    qDeleteAll(_childItems);
}

void TreeItem::appendChild(TreeItem* child)
{
    _childItems.append(child);
}

TreeItem* TreeItem::child(int row)
{
    if (row < 0 || row >= _childItems.size())
        return nullptr;
    return _childItems.at(row);
}

int TreeItem::childCount() const
{
    return _childItems.count();
}

int TreeItem::row() const
{
    if (_parentItem)
        return _parentItem->_childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

int TreeItem::columnCount() const
{
    return _itemData.count();
}

QVariant TreeItem::data(int column) const
{
    if (column < 0 || column >= _itemData.size())
        return {};
    return _itemData.at(column);
}

TreeItem* TreeItem::parentItem()
{
    return _parentItem;
}
