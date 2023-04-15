#ifndef ACCOUNTANCY_IDATAFILEINTERFACE_HPP
#define ACCOUNTANCY_IDATAFILEINTERFACE_HPP

#include <QFile>
#include <QJsonObject>
#include <QList>
#include <QMap>

class Transaction;
class ImportConfig;

class IDataFileInterface
{
public:
    virtual ~IDataFileInterface() = default;
    [[nodiscard]] virtual QList<Transaction*> readTransactionsFromFile(QFile& dataFile, const ImportConfig& config) const = 0;

private:
    int _nbLinesToSkip = 0;
    QMap<QString, QString> _mapFile2Transaction;
};

#endif//ACCOUNTANCY_IDATAFILEINTERFACE_HPP
