#ifndef ACCOUNTANCY_FINANCIALINSTITUTION_HPP
#define ACCOUNTANCY_FINANCIALINSTITUTION_HPP

#include <QFile>
#include <QString>

#include "datafileinterface.hpp"
#include "transaction.hpp"

class FinancialInstitution: public DataFileInterface
{
public:
    FinancialInstitution() = default;
    explicit FinancialInstitution(QString name);
    ~FinancialInstitution() override = default ;

    // Getter & Setter
    [[nodiscard]] int getId() const;
    void setId(int id);
    [[nodiscard]] const QString &getName() const;
    void setName(const QString &name);

    // DataFileInterface
    QList<Transaction*> readTransactionsFromFile(QFile &dataFile) const override;

    // Serialization
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

private:
    int _id = -1;
    QString _name;
};

#endif //ACCOUNTANCY_FINANCIALINSTITUTION_HPP
