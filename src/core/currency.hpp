#ifndef CURRENCY_H
#define CURRENCY_H

#include <QString>

class Currency {
public:
    Currency();
    Currency(QString &name, QString &symbol);
    ~Currency();

    QString getName() const;
    void setName(const QString &value);

    QString getSymbol() const;
    void setSymbol(QString value);

    int getId() const;

private:
    int id = -1;
    QString name;
    QString symbol;
};

#endif // CURRENCY_H
