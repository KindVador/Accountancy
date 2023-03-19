#include "abstractmodel.hpp"

#include <utility>

AbstractModel::AbstractModel(QString name) : _name(std::move(name))
{
}

const QString& AbstractModel::getName()
{
    return _name;
}
