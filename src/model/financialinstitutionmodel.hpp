#ifndef ACCOUNTANCY_FINANCIALINSTITUTIONMODEL_HPP
#define ACCOUNTANCY_FINANCIALINSTITUTIONMODEL_HPP

#include "abstractmodel.hpp"
#include "core/financialinstitution.hpp"

#include <QAbstractItemModel>

class FinancialInstitutionModel : public QAbstractListModel, public AbstractModel
{
    Q_OBJECT
public:
    explicit FinancialInstitutionModel(QString name);
    ~FinancialInstitutionModel() override = default;

    // AbstractModel
    [[nodiscard]] bool isDirty() const override;
    void reset() override;

    // QAbstractListModel interface
    [[nodiscard]] int rowCount(const QModelIndex& parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;

    void addFinancialInstitution(FinancialInstitution* institution);
    FinancialInstitution* addFinancialInstitution(const QString& name);
    void removeFinancialInstitution(const FinancialInstitution* institution);
    void removeFinancialInstitution(QUuid uid);

    [[nodiscard]] FinancialInstitution* getFinancialInstitution(QUuid uid) const;

private:
    QList<FinancialInstitution*> _institutions;
};

#endif//ACCOUNTANCY_FINANCIALINSTITUTIONMODEL_HPP
