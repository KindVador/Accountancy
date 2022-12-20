#ifndef ACCOUNTANCY_FINANCIALINSTITUTION_HPP
#define ACCOUNTANCY_FINANCIALINSTITUTION_HPP

#include <QFile>
#include <QString>
#include <QUuid>

#include "Interfaces/idatafileinterface.hpp"
#include "Interfaces/iserializable.hpp"
#include "transaction.hpp"

class FinancialInstitution : public IDataFileInterface, public ISerializable
{
public:
    FinancialInstitution();
    explicit FinancialInstitution(QString name);
    ~FinancialInstitution() override = default;

    static FinancialInstitution* fromJson(const QJsonObject& json);

    // Getter & Setter
    [[nodiscard]] QUuid getUid() const;
    void setUid(QUuid uid);
    [[nodiscard]] const QString& getName() const;
    void setName(const QString& name);

    // IDataFileInterface
    [[nodiscard]] QList<Transaction*> readTransactionsFromFile(QFile& dataFile, const ImportConfig& config) const override;

    // Serialization
    void read(const QJsonObject& json) override;
    void write(QJsonObject& json) const override;

private:
    QUuid _uid;
    QString _name;
};

#endif//ACCOUNTANCY_FINANCIALINSTITUTION_HPP
