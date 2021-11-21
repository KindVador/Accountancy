
add_library(AccountancyUi
            mainwindow.hpp mainwindow.cpp mainwindow.ui
            owneritemwidget.hpp owneritemwidget.cpp owneritemwidget.ui
            contextualmenugenerator.hpp contextualmenugenerator.cpp
        )

target_link_libraries(AccountancyUi PRIVATE Qt::Widgets)