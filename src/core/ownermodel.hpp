#ifndef OWNERMODEL_H
#define OWNERMODEL_H

#include <QAbstractListModel>
#include <QUuid>
#include <QDebug>

#include "owner.hpp"


class OwnerModel : public QAbstractListModel
{
    Q_OBJECT

public:
    OwnerModel() = default;
    ~OwnerModel() override = default;

    // QAbstractListModel interface
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    void addOwner(Owner *owner);
    Owner *addOwner(const QString &name, float warningBalance, const QString &comment, bool isHidden);
    void removeOwner(const QModelIndex &index);
    [[nodiscard]] Owner* getOwner(const QString &name) const;
    [[nodiscard]] Owner* getOwner(QUuid uid) const;

    void reset();

private:
    QList<Owner*> _owners;
};

#endif // OWNERMODEL_H
