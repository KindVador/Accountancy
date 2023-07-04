set(NAME AccountancyCore)

set(HEADER_FILES
        Interfaces/iserializable.hpp
        account.hpp
        currency.hpp
        loanaccount.hpp
        owner.hpp
        transaction.hpp
        accountfilter.hpp
        financialinstitution.hpp
        category.hpp
        treeitem.hpp
        importconfig.hpp
        )

set(SOURCE_FILES
        account.cpp
        currency.cpp
        loanaccount.cpp
        owner.cpp
        transaction.cpp
        accountfilter.cpp
        financialinstitution.cpp
        category.cpp
        treeitem.cpp
        importconfig.cpp
        )

add_library(${NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(${NAME} PRIVATE Qt::Widgets Qt::Core)
target_include_directories(${NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/..")