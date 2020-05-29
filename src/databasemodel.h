#ifndef DATABASEMODEL_H
#define DATABASEMODEL_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QString>
#include <map>

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
    QSqlTableModel* getModelForTable(QString tableName);

signals:

public slots:
    bool connectToDatabase();
    void disconnectFromDatabase();

private:
    QString dbString;
    QSqlDatabase db;
    std::map<QString, QSqlTableModel*> models;
};

#endif // DATABASEMODEL_H
