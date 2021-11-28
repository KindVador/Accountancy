#ifndef ACCOUNTANCY_BANK_HPP
#define ACCOUNTANCY_BANK_HPP

#include "transaction.hpp"

#include <QFile>
#include <QString>

class Bank
{
public:
    Bank() = default;
    ~Bank() = default;

    QList<Transaction *> dataFileToTransactions(const QFile &inFile);

    [[nodiscard]] int getId() const;
    void setId(int id);

    [[nodiscard]] const QString &getName() const;
    void setName(const QString &name);

private:
    int _id = -1;
    QString _name;
};


#endif //ACCOUNTANCY_BANK_HPP
