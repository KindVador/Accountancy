
add_library(AccountancyUi
            mainwindow.hpp mainwindow.cpp mainwindow.ui
            owneritemwidget.hpp owneritemwidget.cpp owneritemwidget.ui)

target_link_libraries(AccountancyUi PRIVATE Qt::Widgets)