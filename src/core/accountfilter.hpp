#ifndef ACCOUNTANCY_ACCOUNTFILTER_HPP
#define ACCOUNTANCY_ACCOUNTFILTER_HPP

#include <QSortFilterProxyModel>
#include <QUuid>

#include "account.hpp"

class AccountFilter : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit AccountFilter(QObject *parent=nullptr);

    void setActiveOwnerUid(QUuid ownerUid);

protected:
    [[nodiscard]] bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QUuid _activeOwnerUid;
};

#endif //ACCOUNTANCY_ACCOUNTFILTER_HPP
