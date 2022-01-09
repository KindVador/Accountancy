#ifndef OWNERMODEL_H
#define OWNERMODEL_H

#include <QAbstractListModel>
#include <QDebug>

#include "core/owner.hpp"


class OwnerModel : public QAbstractListModel
{
    Q_OBJECT

public:
    OwnerModel() = default;
    ~OwnerModel() override = default;

    // QAbstractListModel interface
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    int addOwner(Owner *owner);
    Owner *addOwner(const QString &name, float warningBalance, const QString &comment, bool isHidden);
    void removeOwner(Owner *owner);
    void removeOwner(int id);
    Owner* getOwner(const QString &name);

private:
    QList<Owner*> _owners;

    [[nodiscard]] int getLastId() const;
};

#endif // OWNERMODEL_H
