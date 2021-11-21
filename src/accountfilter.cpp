#include "accountfilter.hpp"

constexpr const int ObjectRole = Qt::UserRole + 1;

AccountFilter::AccountFilter(QObject *parent) : QSortFilterProxyModel(parent)
{
    qWarning() << "AccountFilter::AccountFilter";
}

bool AccountFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    const QModelIndex modelRequestIdx = sourceModel()->index(sourceRow, 0, sourceParent);
    const Account *account = sourceModel()->data(modelRequestIdx, ObjectRole).value<Account*>();

    if (account == nullptr || _activeOwnerId == -1)
        return true;

    if (account->getOwnersId().contains(_activeOwnerId)) {
        qWarning() << account->getDisplayedName() << true;
        return true;
    } else {
        qWarning() << account->getDisplayedName() << false;
        return false;
    }
}

void AccountFilter::setActiveOwnerId(int ownerId)
{
    qWarning() << "AccountFilter::setActiveOwnerId" << ownerId;
    _activeOwnerId = ownerId;
}
