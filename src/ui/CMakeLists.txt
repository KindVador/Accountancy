
add_library(AccountancyUi
            mainwindow.hpp mainwindow.cpp mainwindow.ui
            owneritemwidget.hpp owneritemwidget.cpp owneritemwidget.ui
            contextualmenugenerator.hpp contextualmenugenerator.cpp
            importdatadialog.hpp importdatadialog.cpp importdatadialog.ui
            transactionswidget.hpp transactionswidget.cpp transactionswidget.ui
            addownerdialog.hpp addownerdialog.cpp addownerdialog.ui
            addaccountdialog.hpp addaccountdialog.cpp addaccountdialog.ui
            currenciesdialog.hpp currenciesdialog.cpp currenciesdialog.ui institutionsdialog.cpp institutionsdialog.hpp institutionsdialog.ui)

target_link_libraries(AccountancyUi PRIVATE Qt::Widgets)
target_include_directories(AccountancyUi PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")