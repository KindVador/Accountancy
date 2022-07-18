#include "importconfig.hpp"

void ImportConfig::read(const QJsonObject &json)
{
    if (json.contains("hasTime") && json["hasTime"].isBool())
        _hasTime = json["hasTime"].toBool();

    if (json.contains("separatorChar") && json["separatorChar"].isString())
        _separatorChar = json["separatorChar"].toString().front();

    if (json.contains("decimalChar") && json["decimalChar"].isString())
        _decimalChar = json["decimalChar"].toString().front();

    if (json.contains("nbLinesToSkipStart") && json["nbLinesToSkipStart"].isDouble())
        _nbLinesToSkipStart = (int) json["nbLinesToSkipStart"].toDouble();

    if (json.contains("nbLinesToSkipEnd") && json["nbLinesToSkipEnd"].isDouble())
        _nbLinesToSkipEnd = (int) json["nbLinesToSkipEnd"].toDouble();

    if (json.contains("dateFormat") && json["dateFormat"].isString())
        _dateFormat = json["dateFormat"].toString();

    if (json.contains("timeFormat") && json["timeFormat"].isString())
        _timeFormat = json["timeFormat"].toString();

    if (json.contains("name") && json["name"].isString())
        _name = json["name"].toString();

    if (json.contains("columns") && json["columns"].isObject()) {
        QJsonObject mapObject = json["columns"].toObject();
        for (auto it = mapObject.begin(); it != mapObject.end(); ++it) {
            _columns.insert(QString(it.key()), (int) it.value().toDouble());
        }
    }
}

void ImportConfig::write(QJsonObject &json) const
{
    json["hasTime"] = _hasTime;
    json["separatorChar"] = QString(_separatorChar);
    json["decimalChar"] = QString(_decimalChar);
    json["nbLinesToSkipStart"] = _nbLinesToSkipStart;
    json["nbLinesToSkipEnd"] = _nbLinesToSkipStart;
    json["dateFormat"] = _dateFormat;
    json["timeFormat"] = _timeFormat;
    json["name"] = _name;
    // QMap<QString, int> to JSON
    QJsonObject columnsObject;
    QMapIterator<QString, int> mapIt(_columns);
    while (mapIt.hasNext()) {
        mapIt.next();
        json.insert(mapIt.key(), mapIt.value());
    }
    json["columns"] = columnsObject;
}

bool ImportConfig::hasTime() const
{
    return _hasTime;
}

void ImportConfig::setHasTime(bool state)
{
    _hasTime = state;
}

const QChar &ImportConfig::getSeparatorChar() const
{
    return _separatorChar;
}

void ImportConfig::setSeparatorChar(const QChar &separator)
{
    _separatorChar = separator;
}

const QChar &ImportConfig::getDecimalChar() const
{
    return _decimalChar;
}

void ImportConfig::setDecimalChar(const QChar &decimal)
{
    _decimalChar = decimal;
}

int ImportConfig::getNbLinesToSkipStart() const
{
    return _nbLinesToSkipStart;
}

void ImportConfig::setNbLinesToSkipStart(int nbLines)
{
    _nbLinesToSkipStart = nbLines;
}

int ImportConfig::getNbLinesToSkipEnd() const
{
    return _nbLinesToSkipEnd;
}

void ImportConfig::setNbLinesToSkipEnd(int nbLines)
{
    _nbLinesToSkipEnd = nbLines;
}

const QString &ImportConfig::getDateFormat() const
{
    return _dateFormat;
}

void ImportConfig::setDateFormat(const QString &format)
{
    _dateFormat = format;
}

const QString &ImportConfig::getTimeFormat() const
{
    return _timeFormat;
}

void ImportConfig::setTimeFormat(const QString &format)
{
    _timeFormat = format;
}

const QString &ImportConfig::getName() const
{
    return _name;
}

void ImportConfig::setName(const QString &newName)
{
    _name = newName;
}

int ImportConfig::getColumnPosition(const QString &columnName) const
{
    return  _columns[columnName];
}

void ImportConfig::addColumn(const QString &columnName, int position)
{
    _columns.insert(columnName, position);
}

void ImportConfig::removeColumn(const QString &columnName)
{
    if (_columns.contains(columnName))
        _columns.remove(columnName);
}

int ImportConfig::nbFields() const
{
    return (int) _columns.size();
}