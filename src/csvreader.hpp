#ifndef CSVREADER_H
#define CSVREADER_H

#include <QFile>
#include <QStringList>
#include <QDebug>

class CsvReader
{
public:
    CsvReader();

    bool parse(QFile &file, char sep);
};

#endif // CSVREADER_H
