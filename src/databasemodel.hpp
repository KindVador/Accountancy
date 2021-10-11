#ifndef DATABASEMODEL_H
#define DATABASEMODEL_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QString>
#include <map>

#include "data_loader/dbmanager.hpp"
#include "currencymodel.hpp"
#include "ownermodel.hpp"

class DatabaseModel : public QObject
{
    Q_OBJECT

public:
    DatabaseModel(QString path, QObject *parent = nullptr);
    ~DatabaseModel();
    void initModel();
    bool isConnected();
    void setDatabaseString(QString dbStr);
    QString getDatabaseString();
    QSqlTableModel* getModelForTable(QString tableName);

    CurrencyModel *getCurrencyModel() const;
    void setCurrencyModel(CurrencyModel *value);

    OwnerModel *getOwnerModel() const;
    void setOwnerModel(OwnerModel *value);

    DbManager *getDbm() const;
    void setDbm(DbManager *value);

    QSqlError addOwner(const QString &name, const QString &currencyName, const float &wngBalance, const QString &comment, const bool &hidden);

signals:

public slots:
    bool connectToDatabase();
    void disconnectFromDatabase();

private:
    QString dbString;
    DbManager* dbm;
    CurrencyModel* currencyModel;
    OwnerModel* ownerModel;

    std::map<QString, QSqlTableModel*> models;
};

#endif // DATABASEMODEL_H
