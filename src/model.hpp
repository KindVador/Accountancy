#ifndef ACCOUNTANCY_MODEL_HPP
#define ACCOUNTANCY_MODEL_HPP

#include "core/owner.hpp"

class Model {

public:
    explicit Model();
    ~Model();

private:
    QList<Owner*> _owners;
};


#endif //ACCOUNTANCY_MODEL_HPP
