#include "csvreader.hpp"

CsvReader::CsvReader()
{

}

bool CsvReader::parse(QFile &file, char sep)
{

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
        return false;
    }

    QStringList wordList;
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        wordList.append(line.split(sep).first());
    }

    qDebug() << wordList;
    return true;
}
