#include "financialinstitution.hpp"

#include <utility>
#include <stdio.h>
#include <QTextStream>

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

    int nbLinesToSkipped = 5;
    int nbLinesRead = 0;
    QTextStream inStream(&dataFile);
    // Reads the data up to the end of file
    while (!inStream.atEnd()) {
        QString line = inStream.readLine();

        // skipped first line
        if (nbLinesRead < nbLinesToSkipped)
            continue;

        QStringList fields = line.split(";");
        // TODO

        ++nbLinesRead;
    }
    dataFile.close();

    return QList<Transaction *>();
}
