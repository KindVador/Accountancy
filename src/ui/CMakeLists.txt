
add_library(ui mainwindow.hpp mainwindow.cpp mainwindow.ui)
target_link_libraries(ui PRIVATE Qt::Widgets Qt::Sql)