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

    QLocale locale = QLocale::system();
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
        QDate date1 = locale.toDate(fields[0], "dd/MM/yy");
        // fix year date as 20 is interpreted as 1920 instead of 2020
        if (date1.year() + 100 < QDate::currentDate().year())
            date1 = date1.addYears(100);
        transaction->setTransactionDate(date1);
        transaction->setValueDate(date1);
        if (!fields[3].isEmpty())
            transaction->setAmount(locale.toDouble(fields[3]));
        else
            transaction->setAmount(locale.toDouble(fields[4]));

        transactions.append(transaction);
    }
    dataFile.close();

    return transactions;
}
