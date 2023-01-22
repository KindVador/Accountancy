
set(SOURCES mainwindow.hpp mainwindow.cpp
        owneritemwidget.hpp owneritemwidget.cpp
        contextualmenugenerator.hpp contextualmenugenerator.cpp
        importdatadialog.hpp importdatadialog.cpp
        transactionswidget.hpp transactionswidget.cpp
        addownerdialog.hpp addownerdialog.cpp
        addaccountdialog.hpp addaccountdialog.cpp
        currenciesdialog.hpp currenciesdialog.cpp
        institutionsdialog.cpp institutionsdialog.hpp
        createcurrencydialog.cpp createcurrencydialog.hpp
        createinstitutiondialog.cpp createinstitutiondialog.hpp
        categoriesdialog.cpp categoriesdialog.hpp
        createcategorydialog.cpp createcategorydialog.hpp
        )

set(UI_SOURCES mainwindow.ui
        owneritemwidget.ui
        importdatadialog.ui
        transactionswidget.ui
        addownerdialog.ui
        addaccountdialog.ui
        currenciesdialog.ui
        institutionsdialog.ui
        createcurrencydialog.ui
        createinstitutiondialog.ui
        categoriesdialog.ui
        createcategorydialog.ui
        )

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    qt_wrap_ui(SOURCES ${UI_SOURCES})
    add_library(AccountancyUi ${SOURCES})
else()
    add_library(AccountancyUi ${SOURCES} ${UI_SOURCES})
endif()

target_link_libraries(AccountancyUi PRIVATE Qt::Widgets)
target_include_directories(AccountancyUi PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")