#ifndef OWNERMODEL_H
#define OWNERMODEL_H

#include <QAbstractListModel>
#include <QDebug>
#include <QUuid>

#include "abstractmodel.hpp"
#include "core/owner.hpp"


class OwnerModel : public QAbstractListModel, public AbstractModel
{
    Q_OBJECT

public:
    explicit OwnerModel(QString name);
    ~OwnerModel() override = default;

    // AbstractModel
    [[nodiscard]] bool isDirty() const override;
    void reset() override;

    // QAbstractListModel interface
    [[nodiscard]] int rowCount(const QModelIndex& parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;

    void addOwner(Owner* owner);
    Owner* addOwner(const QString& name, double warningBalance, const QString& comment, bool isHidden);
    void removeOwner(const QModelIndex& index);
    [[nodiscard]] Owner* getOwner(const QString& name) const;
    [[nodiscard]] Owner* getOwner(QUuid uid) const;


private:
    QList<Owner*> _owners;
};

#endif// OWNERMODEL_H
