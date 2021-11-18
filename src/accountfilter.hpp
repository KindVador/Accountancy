#ifndef ACCOUNTANCY_ACCOUNTFILTER_HPP
#define ACCOUNTANCY_ACCOUNTFILTER_HPP

#include <QSortFilterProxyModel>
#include "core/account.hpp"

class AccountFilter : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit AccountFilter(QObject *parent=nullptr);

    void setActiveOwnerId(int ownerId);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    int _activeOwnerId = -1;
};

#endif //ACCOUNTANCY_ACCOUNTFILTER_HPP
