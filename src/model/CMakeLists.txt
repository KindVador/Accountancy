set(NAME AccountancyModel)

set(HEADER_FILES
        abstractmodel.hpp
        currencymodel.hpp
        ownermodel.hpp
        accountmodel.hpp
        financialinstitutionmodel.hpp
        transactionmodel.hpp
        categorymodel.hpp
        importconfigmodel.hpp
        model.hpp
        treemodel.hpp
        categorymodelfilter.hpp)

set(SOURCE_FILES
        abstractmodel.cpp
        currencymodel.cpp
        ownermodel.cpp
        accountmodel.cpp
        financialinstitutionmodel.cpp
        transactionmodel.cpp
        categorymodel.cpp
        importconfigmodel.cpp
        model.cpp
        treemodel.cpp
        categorymodelfilter.cpp)

add_library(${NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(${NAME} PRIVATE Qt::Core Qt::Gui AccountancyCore)
target_include_directories(${NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/..")