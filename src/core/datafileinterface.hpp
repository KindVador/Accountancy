#ifndef ACCOUNTANCY_DATAFILEINTERFACE_HPP
#define ACCOUNTANCY_DATAFILEINTERFACE_HPP

#include <QList>
#include <QFile>

class Transaction;

class DataFileInterface
{
public:
    virtual ~DataFileInterface() = default;
    [[nodiscard]] virtual QList<Transaction*> readTransactionsFromFile(QFile &dataFile) = 0;
};


#endif //ACCOUNTANCY_DATAFILEINTERFACE_HPP
