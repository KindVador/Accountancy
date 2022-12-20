#ifndef ACCOUNTANCY_ISERIALIZABLE_HPP
#define ACCOUNTANCY_ISERIALIZABLE_HPP

class QJsonObject;

class ISerializable
{
public:
    virtual ~ISerializable() = default;

    virtual void read(const QJsonObject& json) = 0;
    virtual void write(QJsonObject& json) const = 0;
};

#endif//ACCOUNTANCY_ISERIALIZABLE_HPP
