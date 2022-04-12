#ifndef ACCOUNTANCY_FINANCIALINSTITUTIONMODEL_HPP
#define ACCOUNTANCY_FINANCIALINSTITUTIONMODEL_HPP

#include "financialinstitution.hpp"

#include <QAbstractItemModel>

class FinancialInstitutionModel : public QAbstractListModel
{
    Q_OBJECT
public:
    FinancialInstitutionModel() = default;
    ~FinancialInstitutionModel() override = default;

    // QAbstractListModel interface
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    void addFinancialInstitution(FinancialInstitution *institution);
    FinancialInstitution *addFinancialInstitution(const QString &name);
    void removeFinancialInstitution(FinancialInstitution *institution);
    void removeFinancialInstitution(QUuid uid);

    [[nodiscard]] FinancialInstitution* getFinancialInstitution(QUuid uid) const;

    void reset();

private:
    QList<FinancialInstitution *> _institutions;
};

#endif //ACCOUNTANCY_FINANCIALINSTITUTIONMODEL_HPP
