
add_library(AccountancyUi
            mainwindow.hpp mainwindow.cpp mainwindow.ui
            owneritemwidget.hpp owneritemwidget.cpp owneritemwidget.ui
            contextualmenugenerator.hpp contextualmenugenerator.cpp
            importdatadialog.hpp importdatadialog.cpp importdatadialog.ui
        )

target_link_libraries(AccountancyUi PRIVATE Qt::Widgets)