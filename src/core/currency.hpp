#ifndef CURRENCY_H
#define CURRENCY_H

#include <QString>
#include <QJsonObject>

class Currency
{
public:
    Currency() = default;
    explicit Currency(QString name, QString symbol);
    ~Currency() = default;

    static Currency *fromJson(const QJsonObject &json);

    // Getter & Setter
    [[nodiscard]] QString getName() const;
    void setName(const QString &value);
    [[nodiscard]] QString getSymbol() const;
    void setSymbol(QString value);
    [[nodiscard]] int getId() const;
    void setId(int id);

    // Displayed Data for Model
    [[nodiscard]] QString getDisplayedName() const;

    // Serialization
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

private:
    int _id = -1;
    QString _name;
    QString _symbol;
};

Q_DECLARE_METATYPE(Currency*)

#endif // CURRENCY_H
