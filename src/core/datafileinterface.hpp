#ifndef ACCOUNTANCY_DATAFILEINTERFACE_HPP
#define ACCOUNTANCY_DATAFILEINTERFACE_HPP

#include <QList>
#include <QFile>
#include <QMap>
#include <QJsonObject>

class Transaction;

struct ImportConfig {
    bool hasTime = false;
    QChar separatorChar{';'};
    QChar decimalChar{','};
    int nbLinesToSkip = 0;
    QString dateFormat{"dd/MM/yy"};
    QString timeFormat{"HH.mm.ss.zzz"};
    QString name;
    QMap<QString, int> columns;

    // Serialization
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
};

void ImportConfig::read(const QJsonObject &json)
{
    if (json.contains("hasTime") && json["hasTime"].isBool())
        hasTime = json["hasTime"].toBool();

    if (json.contains("separatorChar") && json["separatorChar"].isString())
        separatorChar = json["separatorChar"].toString().front();

    if (json.contains("decimalChar") && json["decimalChar"].isString())
        decimalChar = json["decimalChar"].toString().front();

    if (json.contains("nbLinesToSkip") && json["nbLinesToSkip"].isDouble())
        nbLinesToSkip = (int) json["nbLinesToSkip"].toDouble();

    if (json.contains("dateFormat") && json["dateFormat"].isString())
        dateFormat = json["dateFormat"].toString();

    if (json.contains("timeFormat") && json["timeFormat"].isString())
        timeFormat = json["timeFormat"].toString();

    if (json.contains("name") && json["name"].isString())
        name = json["name"].toString();

    if (json.contains("columns") && json["columns"].isObject()) {
        QJsonObject mapObject = json["columns"].toObject();
        for (auto it = mapObject.begin(); it != mapObject.end(); ++it) {
            columns.insert(QString(it.key()), (int) it.value().toDouble());
        }
    }

}

void ImportConfig::write(QJsonObject &json) const
{
    json["hasTime"] = hasTime;
    json["separatorChar"] = QString(separatorChar);
    json["decimalChar"] = QString(decimalChar);
    json["nbLinesToSkip"] = nbLinesToSkip;
    json["dateFormat"] = dateFormat;
    json["timeFormat"] = timeFormat;
    json["name"] = name;
    // QMap<QString, int> to JSON
    QJsonObject columnsObject;
    QMapIterator<QString, int> mapIt(columns);
    while (mapIt.hasNext()) {
        mapIt.next();
        json.insert(mapIt.key(), mapIt.value());
    }
    json["columns"] = columnsObject;
}

class DataFileInterface
{
public:
    virtual ~DataFileInterface() = default;
    [[nodiscard]] virtual QList<Transaction *> readTransactionsFromFile(QFile &dataFile) const = 0;

private:
    int _nbLinesToSkip = 0;
    QMap<QString, QString> _mapFile2Transaction;
};

#endif //ACCOUNTANCY_DATAFILEINTERFACE_HPP
