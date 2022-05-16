#include "financialinstitution.hpp"

#include <utility>
#include <QTextStream>
#include <QDebug>

FinancialInstitution::FinancialInstitution()
{
    _uid = QUuid::createUuid();
}

FinancialInstitution::FinancialInstitution(QString name): _name(std::move(name))
{
    _uid = QUuid::createUuid();
}

QUuid FinancialInstitution::getUid() const
{
    return _uid;
}

void FinancialInstitution::setUid(QUuid uid)
{
    _uid = uid;
}

const QString &FinancialInstitution::getName() const
{
    return _name;
}

void FinancialInstitution::setName(const QString &name)
{
    _name = name;
}

QList<Transaction *> FinancialInstitution::readTransactionsFromFile(QFile &dataFile) const
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
        QDate date = locale.toDate(fields[0], "dd/MM/yy");
        // fix year date as 20 is interpreted as 1920 instead of 2020
        if (date.year() + 100 <= QDate::currentDate().year())
            date = date.addYears(100);
        QTime time;
        if (fields[1].length() > 28) {
            QString timeString = fields[1].mid(17, 12);
            time = QTime::fromString(timeString, "HH.mm.ss.zzz");
        }
        transaction->setDateTime(QDateTime(date, time));
        if (!fields[3].isEmpty())
            transaction->setAmount(locale.toDouble(fields[3]));
        else
            transaction->setAmount(locale.toDouble(fields[4]));

        transactions.append(transaction);
    }
    dataFile.close();

    return transactions;
}

void FinancialInstitution::read(const QJsonObject &json)
{
    if (json.contains("uid") && json["uid"].isString())
        _uid = QUuid(json["uid"].toString());

    if (json.contains("name") && json["name"].isString())
        _name = json["name"].toString();
}

void FinancialInstitution::write(QJsonObject &json) const
{
    json["uid"] = _uid.toString();
    json["name"] = _name;
}

FinancialInstitution *FinancialInstitution::fromJson(const QJsonObject &json)
{
    if (json.isEmpty())
        return nullptr;

    auto fi = new FinancialInstitution;
    fi->read(json);
    return fi;
}
