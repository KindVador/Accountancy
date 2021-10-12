#ifndef OWNERMODEL_H
#define OWNERMODEL_H

#include <QSql>
#include <QSqlQuery>
#include <QSqlRelationalTableModel>
#include <QSqlError>
#include <QDebug>
#include <vector>

#include "core/owner.hpp"


class OwnerModel : public QSqlRelationalTableModel
{
public:
    OwnerModel();
    QSqlError addOwner(const Owner &owner);
    QSqlError addOwner(const QString &name, const int &currencyId, const float &wngBalance, const QString &comment, const bool &hidden);
    std::vector<Owner> getOwners();
};

#endif // OWNERMODEL_H
