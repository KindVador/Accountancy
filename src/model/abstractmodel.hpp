#ifndef ACCOUNTANCY_ABSTRACTMODEL_HPP
#define ACCOUNTANCY_ABSTRACTMODEL_HPP

#include <QJsonObject>
#include <QString>

/**
 * @brief Abstract model and interface
 * It allows use of Abstraction to interacts with all the models contained in the application like CategoryModel, CurrenciesModel, ...
 */
class AbstractModel
{
public:
    explicit AbstractModel(QString name);
    virtual ~AbstractModel() = default;

    /**
     * @brief returns model's name
     * @return QString with model's name
     */
    virtual const QString& getName() const;
    /**
     * @brief tells if model has unsaved modification
     * @return returns true if model has unsaved modification otherwise false
     */
    [[nodiscard]] virtual bool isDirty() const = 0;
    /**
     * @brief reset the model.
     */
    virtual void reset() = 0;
    /**
     * @brief
     * @param json
     */
    virtual void write(QJsonObject& json) const = 0;
    /**
     * @brief
     * @param json
     */
    virtual void read(const QJsonObject& json) = 0;

protected:
    QString _name;
};


#endif//ACCOUNTANCY_ABSTRACTMODEL_HPP
