#ifndef ACCOUNTANCY_CATEGORYMODELFILTER_HPP
#define ACCOUNTANCY_CATEGORYMODELFILTER_HPP

#include <QSortFilterProxyModel>

class CategoryModelFilter : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit CategoryModelFilter(QObject* parent);

protected:
    [[nodiscard]] bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
};

#endif//ACCOUNTANCY_CATEGORYMODELFILTER_HPP
