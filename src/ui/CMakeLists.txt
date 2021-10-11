
add_library(AccountancyUi mainwindow.hpp mainwindow.cpp mainwindow.ui)
target_link_libraries(AccountancyUi PRIVATE Qt::Widgets Qt::Sql)