#include "src/sql_models/ownermodel.hpp"


const auto INSERT_OWNER_SQL = QLatin1String(R"(INSERT INTO owners(name, currency, warningbalance, comment, ishidden) values(:name, :currency, :warningbalance ,:comment, :ishidden))");
const auto SELECT_SQL = QLatin1String(R"(SELECT * FROM owners)");

OwnerModel::OwnerModel()
{
    this->setTable("owners");
    this->setRelation(2, QSqlRelation("currencies", "id", "names"));
}

QSqlError OwnerModel::addOwner(const Owner &owner)
{
    return this->addOwner(owner.getName(), owner.getCurrency().getId(), owner.getWarningBalance(), owner.getComment(), owner.getIsHidden());
}

QSqlError OwnerModel::addOwner(const QString &name, const int &currencyId, const float &wngBalance, const QString &comment, const bool &hidden)
{
    QSqlQuery query;
    query.prepare(INSERT_OWNER_SQL);
    query.bindValue(":name", name);
    query.bindValue(":currency", currencyId);
    query.bindValue(":wngbalance", wngBalance);
    query.bindValue(":comment", comment);
    query.bindValue(":ishidden", hidden);
    if(query.exec()) {
        return QSqlError();
    } else {
        QSqlError err = query.lastError();
        qDebug() << "addCurrency error:" << err;
        return err;
    }
}

std::vector<Owner> OwnerModel::getOwners()
{
    qDebug() << "std::vector<Owner> OwnerModel::getOwners()";
    std::vector<Owner> owners;
    QSqlQueryModel model;
    model.setQuery(SELECT_SQL);
    qDebug() << "model.rowCount() = "<< model.rowCount();
    for (int i = 0; i < model.rowCount(); ++i) {
        Owner o;
        o.setName(model.record(i).value("name").toString());
//        o.setCurrency(model.record(i).value("currency").toInt());
        o.setWarningBalance(model.record(i).value("warningbalance").toReal());
        o.setComment(model.record(i).value("comment").toString());
        o.setIsHidden(model.record(i).value("").toBool());
        owners.push_back(o);
    }
    return owners;
}
