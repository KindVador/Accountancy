#ifndef ACCOUNTANCY_FINANCIALINSTITUTION_HPP
#define ACCOUNTANCY_FINANCIALINSTITUTION_HPP

#include <QFile>
#include <QString>
#include <QUuid>

#include "datafileinterface.hpp"
#include "transaction.hpp"

class FinancialInstitution: public DataFileInterface
{
public:
    FinancialInstitution();
    explicit FinancialInstitution(QString name);
    ~FinancialInstitution() override = default ;

    static FinancialInstitution *fromJson(const QJsonObject &json);

    // Getter & Setter
    [[nodiscard]] QUuid getUid() const;
    void setUid(QUuid uid);
    [[nodiscard]] const QString &getName() const;
    void setName(const QString &name);

    // DataFileInterface
    [[nodiscard]] QList<Transaction*> readTransactionsFromFile(QFile &dataFile, const ImportConfig &config) const override;

    // Serialization
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

private:
    QUuid _uid;
    QString _name;
};

#endif //ACCOUNTANCY_FINANCIALINSTITUTION_HPP
