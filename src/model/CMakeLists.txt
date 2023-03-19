set(NAME AccountancyModel)

set(HEADER_FILES
        currencymodel.hpp
        ownermodel.hpp
        accountmodel.hpp
        financialinstitutionmodel.hpp
        transactionmodel.hpp
        model.hpp
        importconfigmodel.hpp
        categorymodel.hpp
        treemodel.hpp
        abstractmodel.hpp)

set(SOURCE_FILES
        currencymodel.cpp
        ownermodel.cpp
        accountmodel.cpp
        financialinstitutionmodel.cpp
        transactionmodel.cpp
        model.cpp
        importconfigmodel.cpp
        categorymodel.cpp
        treemodel.cpp
        abstractmodel.cpp)

add_library(${NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(${NAME} PRIVATE Qt::Core Qt::Gui AccountancyCore)
target_include_directories(${NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/..")