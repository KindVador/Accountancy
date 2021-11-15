#ifndef ACCOUNTANCY_ACCOUNTFILTER_HPP
#define ACCOUNTANCY_ACCOUNTFILTER_HPP

#include <QSortFilterProxyModel>

class AccountFilter : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    AccountFilter() = default;
};

#endif //ACCOUNTANCY_ACCOUNTFILTER_HPP
