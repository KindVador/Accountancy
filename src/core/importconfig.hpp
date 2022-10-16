#ifndef ACCOUNTANCY_IMPORTCONFIG_HPP
#define ACCOUNTANCY_IMPORTCONFIG_HPP

#include <QUuid>
#include <QString>
#include <QJsonObject>
#include <QChar>

class ImportConfig
{
public:
    ImportConfig();
    ~ImportConfig() = default;

    static ImportConfig* fromJson(const QJsonObject & json);

    // Serialization
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    // Getter & Setter
    [[nodiscard]] QUuid getUid() const;
    void setUid(QUuid id);

    [[nodiscard]] bool hasTime() const;
    void setHasTime(bool state);

    [[nodiscard]] const QChar &getSeparatorChar() const;
    void setSeparatorChar(const QChar &separator);

    [[nodiscard]] const QChar &getDecimalChar() const;
    void setDecimalChar(const QChar &decimal);

    [[nodiscard]] int getNbLinesToSkipStart() const;
    void setNbLinesToSkipStart(int nbLines);

    [[nodiscard]] int getNbLinesToSkipEnd() const;
    void setNbLinesToSkipEnd(int nbLines);

    [[nodiscard]] const QString &getDateFormat() const;
    void setDateFormat(const QString &format);

    [[nodiscard]] const QString &getTimeFormat() const;
    void setTimeFormat(const QString &format);

    [[nodiscard]] const QString &getName() const;
    void setName(const QString &newName);

    // Public API to configure columns
    [[nodiscard]] int getColumnPosition(const QString &columnName) const;
    void addColumn(const QString &columnName, int position);
    void removeColumn(const QString &columnName);
    [[nodiscard]] int nbFields() const;

private:
    QUuid _uid;
    bool _hasTime = false;
    QChar _separatorChar{';'};
    QChar _decimalChar{','};
    int _nbLinesToSkipStart = 0;
    int _nbLinesToSkipEnd = 0;
    QString _dateFormat{"dd/MM/yy"};
    QString _timeFormat{"HH.mm.ss.zzz"};
    QString _name;
    QMap<QString, int> _columns;
};

Q_DECLARE_METATYPE(ImportConfig*)

#endif //ACCOUNTANCY_IMPORTCONFIG_HPP
