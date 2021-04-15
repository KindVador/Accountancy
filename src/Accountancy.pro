QT += core gui
QT += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES+="ACC_MAJOR_VERSION=1"
DEFINES+="ACC_MINOR_VERSION=0"
DEFINES+="ACC_PATCH_VERSION=0"

CONFIG += c++17

# add this option to silence warnings related to SDK with macOS 11.x (Big Sur)
CONFIG += sdk_no_version_check

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    account.cpp \
    currency.cpp \
    databasemodel.cpp \
    dbtablewidget.cpp \
    main.cpp \
    maindockwidget.cpp \
    mainwindow.cpp \
    owner.cpp \
    transaction.cpp

HEADERS += \
    account.h \
    currency.h \
    databasemodel.h \
    dbtablewidget.h \
    maindockwidget.h \
    mainwindow.h \
    owner.h \
    transaction.h

FORMS += \
    ImportDialogInDb.ui \
    dbtablewidget.ui \
    maindockwidget.ui \
    mainwindow.ui

TRANSLATIONS += \
    Accountancy_fr_FR.ts \
    Accountancy_en_US.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Accountancy_en_US.ts \
    Accountancy_fr_FR.ts

DESTDIR = ./dist

RESOURCES += \
    Resources.qrc
