set(HEADER_FILES
        account.hpp
        currency.hpp
        loanaccount.hpp
        owner.hpp
        transaction.hpp
        currencymodel.hpp
        ownermodel.hpp
        accountmodel.hpp
        accountfilter.hpp
        financialinstitutionmodel.hpp
        transactionmodel.hpp
        datafileinterface.hpp
        financialinstitution.hpp
        model.hpp
        controller.hpp
        )

set(SOURCE_FILES
        account.cpp
        currency.cpp
        loanaccount.cpp
        owner.cpp
        transaction.cpp
        currencymodel.cpp
        ownermodel.cpp
        accountmodel.cpp
        accountfilter.cpp
        financialinstitutionmodel.cpp
        transactionmodel.cpp
        financialinstitution.cpp
        model.cpp
        controller.cpp
        )

add_library(AccountancyCore ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(AccountancyCore PRIVATE Qt::Widgets Qt::Core)
target_include_directories(AccountancyCore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")