#include "financialinstitution.hpp"

#include <utility>
#include <QTextStream>
#include <QDebug>

FinancialInstitution::FinancialInstitution(QString name): _name(std::move(name))
{
}

int FinancialInstitution::getId() const
{
    return _id;
}

void FinancialInstitution::setId(int id)
{
    _id = id;
}

const QString &FinancialInstitution::getName() const
{
    return _name;
}

void FinancialInstitution::setName(const QString &name)
{
    _name = name;
}

QList<Transaction *> FinancialInstitution::readTransactionsFromFile(QFile &dataFile)
{
    if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    QList<Transaction *> transactions;
    int nbLinesToSkipped = 6;
    int nbLinesRead = 0;
    QTextStream inStream(&dataFile);
    // Reads the data up to the end of file
    while (!inStream.atEnd()) {
        QString line = inStream.readLine();
        ++nbLinesRead;
        QStringList fields = line.split(';');

        // skipped first line
        if (nbLinesRead < nbLinesToSkipped || fields.count() < 6)
            continue;

        auto *transaction = new Transaction();
        transaction->setName(fields[2]);
        transaction->setComment(fields[5]);
        transaction->setTransactionDate(QDate::fromString(fields[0], "DD/MM/YY"));
        transaction->setValueDate(QDate::fromString(fields[0], "DD/MM/YY"));
        if (!fields[3].isEmpty())
            transaction->setAmount(fields[3].toDouble());
        else
            transaction->setAmount(fields[4].toDouble());

        transactions.append(transaction);
    }
    dataFile.close();

    return transactions;
}
