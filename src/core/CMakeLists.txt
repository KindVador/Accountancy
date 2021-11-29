
add_library(AccountancyCore
            financialinstitution.hpp financialinstitution.cpp
            account.hpp account.cpp
            currency.hpp currency.cpp
            loanaccount.hpp loanaccount.cpp
            owner.hpp owner.cpp
            transaction.hpp transaction.cpp)

target_link_libraries(AccountancyCore PRIVATE Qt::Widgets Qt::Core)