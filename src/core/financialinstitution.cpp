#include "financialinstitution.hpp"
#include "importconfig.hpp"

#include <QDebug>
#include <QTextStream>
#include <utility>

FinancialInstitution::FinancialInstitution()
{
    _uid = QUuid::createUuid();
}

FinancialInstitution::FinancialInstitution(QString name) : _name(std::move(name))
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

const QString& FinancialInstitution::getName() const
{
    return _name;
}

void FinancialInstitution::setName(const QString& name)
{
    _name = name;
}

QList<Transaction*> FinancialInstitution::readTransactionsFromFile(QFile& dataFile, const ImportConfig& config) const
{
    if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    QLocale locale = QLocale::system();
    bool replaceDecimalCharacter = config.getDecimalChar() != locale.decimalPoint();

    QList<Transaction*> transactions;
    QVector<QString> rawLines;
    QTextStream inStream(&dataFile);
    // Reads the data up to the end of file
    while (!inStream.atEnd())
        rawLines.append(inStream.readLine());

    int firstLineToImport = config.getNbLinesToSkipStart() == 0 ? 0 : config.getNbLinesToSkipStart();
    int nbLinesToImport = (int) rawLines.count() - (config.getNbLinesToSkipStart() + config.getNbLinesToSkipEnd()) + 1;
    rawLines = rawLines.mid(firstLineToImport, nbLinesToImport);

    for (int i = 0; i < rawLines.count(); ++i) {
        QStringList fields = rawLines[i].split(config.getSeparatorChar());

        if (fields.count() < config.nbFields())
            continue;

        auto* transaction = new Transaction();
        transaction->setName(fields[config.getColumnPosition("Name")]);
        transaction->setComment(fields[config.getColumnPosition("Comment")]);
        QDate date = locale.toDate(fields[config.getColumnPosition("Date")], config.getDateFormat());
        // fix year date as 20 is interpreted as 1920 instead of 2020
        if (date.year() + 100 <= QDate::currentDate().year())
            date = date.addYears(100);
        QTime time;
        if (config.hasTime()) {
            QString timeString = fields[config.getColumnPosition("Time")];
            const QString& timeFormat = config.getTimeFormat();
            if ((timeString.length() > timeFormat.length()) && timeString.length() > 28)
                timeString = timeString.mid(17, 12);
            time = QTime::fromString(timeString, timeFormat);
        }
        transaction->setDateTime(QDateTime(date, time));

        QString amountValue;
        if (!fields[config.getColumnPosition("DebitAmount")].isEmpty())
            amountValue = fields[config.getColumnPosition("DebitAmount")];
        else
            amountValue = fields[config.getColumnPosition("CreditAmount")];

        if (replaceDecimalCharacter)
            amountValue.replace(config.getDecimalChar(), locale.decimalPoint());

        transaction->setAmount(locale.toDouble(amountValue));

        transactions.append(transaction);
    }
    dataFile.close();

    return transactions;
}

void FinancialInstitution::read(const QJsonObject& json)
{
    if (json.contains("uid") && json["uid"].isString())
        _uid = QUuid(json["uid"].toString());

    if (json.contains("name") && json["name"].isString())
        _name = json["name"].toString();
}

void FinancialInstitution::write(QJsonObject& json) const
{
    json["uid"] = _uid.toString();
    json["name"] = _name;
}

FinancialInstitution* FinancialInstitution::fromJson(const QJsonObject& json)
{
    if (json.isEmpty())
        return nullptr;

    auto fi = new FinancialInstitution;
    fi->read(json);
    return fi;
}
