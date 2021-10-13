#ifndef OWNERMODEL_H
#define OWNERMODEL_H

#include <QDebug>

#include "core/owner.hpp"


class OwnerModel {
public:
    OwnerModel();
    ~OwnerModel();

    void addOwner(Owner* owner);
    void addOwner(const QString &name, const Currency &currency, float warningBalance, const QString &comment, bool isHidden);
    void removeOwner(Owner* owner);
    void removeOwner(int id);
    Owner* getOwner(const QString& name);

private:
    QMap<int, Owner*> _owners;

};

#endif // OWNERMODEL_H
