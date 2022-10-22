#ifndef ACCOUNTANCY_DATAFILEINTERFACE_HPP
#define ACCOUNTANCY_DATAFILEINTERFACE_HPP

#include <QFile>
#include <QJsonObject>
#include <QList>
#include <QMap>

class Transaction;
class ImportConfig;

class DataFileInterface
{
public:
    virtual ~DataFileInterface() = default;
    [[nodiscard]] virtual QList<Transaction*> readTransactionsFromFile(QFile& dataFile, const ImportConfig& config) const = 0;

private:
    int _nbLinesToSkip = 0;
    QMap<QString, QString> _mapFile2Transaction;
};

#endif//ACCOUNTANCY_DATAFILEINTERFACE_HPP
