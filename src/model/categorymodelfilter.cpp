#include "categorymodelfilter.hpp"

CategoryModelFilter::CategoryModelFilter(QObject* parent) : QSortFilterProxyModel(parent)
{
}

// https://forum.qt.io/topic/65733/qtreeview-filtering-search
bool CategoryModelFilter::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    // check the current item
    bool result = QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
    QModelIndex currentIndex = sourceModel()->index(source_row, 0, source_parent);
    if (sourceModel()->hasChildren(currentIndex)) {
        // if it has sub items
        for (int i = 0; i < sourceModel()->rowCount(currentIndex) && !result; ++i) {
            // keep the parent if a children is shown
            result = result || filterAcceptsRow(i, currentIndex);
        }
    }
    return result;
}
