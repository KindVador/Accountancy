#ifndef ACCOUNTANCY_FINANCIALINSTITUTION_HPP
#define ACCOUNTANCY_FINANCIALINSTITUTION_HPP

#include <QFile>
#include <QString>

class Transaction;

class FinancialInstitution
{
public:
    FinancialInstitution() = default;
    explicit FinancialInstitution(QString name);
    ~FinancialInstitution() = default;

    QList<Transaction *> dataFileToTransactions(const QFile &inFile);

    [[nodiscard]] int getId() const;
    void setId(int id);

    [[nodiscard]] const QString &getName() const;
    void setName(const QString &name);

private:
    int _id = -1;
    QString _name;
};


#endif //ACCOUNTANCY_FINANCIALINSTITUTION_HPP
