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

    if (account == nullptr || _activeOwnerUid.isNull())
        return true;

    if (account->getOwnersUid().contains(_activeOwnerUid)) {
        qWarning() << account->getDisplayedName() << true;
        return true;
    } else {
        qWarning() << account->getDisplayedName() << false;
        return false;
    }
}

void AccountFilter::setActiveOwnerUid(QUuid ownerUid)
{
    qWarning() << "AccountFilter::setActiveOwnerUid" << ownerUid.toString();
    beginResetModel();
    _activeOwnerUid = ownerUid;
    endResetModel();
}
