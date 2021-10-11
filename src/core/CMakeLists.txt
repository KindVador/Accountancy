
add_library(AccountancyCore
            account.hpp account.cpp
            card.hpp card.cpp
            checkbook.hpp checkbook.cpp
            currency.hpp currency.cpp
            loanaccount.hpp loanaccount.cpp
            owner.hpp owner.cpp
            transaction.hpp transaction.cpp)

target_link_libraries(AccountancyCore PRIVATE Qt::Widgets Qt::Core)