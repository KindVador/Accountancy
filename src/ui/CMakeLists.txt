set(NAME AccountancyUi)

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
        comboboxitemdelegate.cpp comboboxitemdelegate.hpp
        categoriestreedialog.cpp categoriestreedialog.hpp
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
        categoriestreedialog.ui
        )

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    qt_wrap_ui(SOURCES ${UI_SOURCES})
    add_library(${NAME} ${SOURCES})
else()
    add_library(${NAME} ${SOURCES} ${UI_SOURCES})
endif()

target_link_libraries(${NAME} PRIVATE Qt::Widgets AccountancyController AccountancyModel)
target_include_directories(${NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/..")