
add_library(AccountancyDataLoader
            dbmanager.hpp dbmanager.cpp)

target_link_libraries(AccountancyDataLoader PRIVATE Qt::Core Qt::Sql)