#ifndef ACCOUNTANCY_FINANCIALINSTITUTION_HPP
#define ACCOUNTANCY_FINANCIALINSTITUTION_HPP

#include <QFile>
#include <QString>

#include "datafileinterface.hpp"
#include "transaction.hpp"

class FinancialInstitution: public DataFileInterface
{
public:
    explicit FinancialInstitution(QString name);
    ~FinancialInstitution() override = default ;

    // DataFileInterface
    QList<Transaction*> readTransactionsFromFile(QFile &dataFile) const override;

    [[nodiscard]] int getId() const;
    void setId(int id);

    [[nodiscard]] const QString &getName() const;
    void setName(const QString &name);

private:
    int _id = -1;
    QString _name;
};

#endif //ACCOUNTANCY_FINANCIALINSTITUTION_HPP
