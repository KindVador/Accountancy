
add_library(AccountancyUi
            mainwindow.hpp mainwindow.cpp mainwindow.ui
            dbtablewidget.hpp dbtablewidget.cpp dbtablewidget.ui
        )

target_link_libraries(AccountancyUi PRIVATE Qt::Widgets Qt::Sql)