#ifndef ACCOUNTANCY_FINANCIALINSTITUTIONMODEL_HPP
#define ACCOUNTANCY_FINANCIALINSTITUTIONMODEL_HPP

#include "core/financialinstitution.hpp"

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

    int addFinancialInstitution(FinancialInstitution *institution);
    int addFinancialInstitution(QString name);

private:
    QList<FinancialInstitution *> _institutions;
};


#endif //ACCOUNTANCY_FINANCIALINSTITUTIONMODEL_HPP