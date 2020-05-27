#ifndef DATABASEMODEL_H
#define DATABASEMODEL_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QString>

class DatabaseModel : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseModel(QObject *parent = nullptr);
    DatabaseModel(QString dbStr, QObject *parent = nullptr);
    ~DatabaseModel();
    void initModel();
    bool isConnected();
    void setDatabaseString(QString dbStr);
    QString getDatabaseString();
    QSqlTableModel* getOwnerModel();

signals:

public slots:
    bool connectToDatabase();
    void disconnectFromDatabase();

private:
    QString dbString;
    QSqlDatabase db;
    QSqlTableModel *ownerModel;
    QSqlTableModel *bankModel;
    QSqlTableModel *accountModel;
    QSqlTableModel *categoryModel;
    QSqlTableModel *retailerModel;
    QSqlTableModel *subcategoryModel;
    QSqlTableModel *transactionModel;
};

#endif // DATABASEMODEL_H
