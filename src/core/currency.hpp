#ifndef CURRENCY_H
#define CURRENCY_H

#include <QString>
#include <QJsonObject>
#include <QUuid>

class Currency
{
public:
    Currency();
    explicit Currency(QString name, QString symbol);
    ~Currency() = default;

    static Currency *fromJson(const QJsonObject &json);

    // Getter & Setter
    [[nodiscard]] QString getName() const;
    void setName(const QString &value);
    [[nodiscard]] QString getSymbol() const;
    void setSymbol(QString value);
    [[nodiscard]] QUuid getUid() const;
    void setUid(QUuid uid);

    // Displayed Data for Model
    [[nodiscard]] QString getDisplayedName() const;

    // Serialization
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

private:
    QUuid _uid;
    QString _name;
    QString _symbol;
};

Q_DECLARE_METATYPE(Currency*)

#endif // CURRENCY_H
