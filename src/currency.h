#ifndef CURRENCY_H
#define CURRENCY_H

#include <QString>

class Currency
{
public:
    Currency();
    Currency(QString &name, char &symbol);
    ~Currency();

private:
    QString name;
    char symbol;
};

#endif // CURRENCY_H
