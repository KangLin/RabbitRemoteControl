TEMPLATE = app
TARGET = RabbitRemoteControl

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
android: QT += androidextras

CONFIG += c++11 link_pkgconfig link_prl

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

isEmpty(DESTDIR): DESTDIR = $$OUT_PWD/../bin

CONFIG(debug, debug|release): DEFINES *= _DEBUG
!static: !staticlib: DEFINES *= BUILD_SHARED_LIBS

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    Resource/Resource.qrc
WIN32: RESOURCES += Resource/App.rc

OTHER_FILES += CMakeLists.txt

isEmpty(RabbitCommon_DIR): RabbitCommon_DIR=$$(RabbitCommon_DIR)
isEmpty(RabbitCommon_DIR): RabbitCommon_DIR=$$PWD/../../RabbitCommon
!isEmpty(RabbitCommon_DIR): exists("$${RabbitCommon_DIR}/Src/RabbitCommon.pri") {
    include($${RabbitCommon_DIR}/pri/Translations.pri)
    INCLUDEPATH *= $${RabbitCommon_DIR}/Src $${RabbitCommon_DIR}/Src/export
    LIBS += -L$$DESTDIR -lRabbitCommon
} else {
    message("1. Please download RabbitCommon source code from https://github.com/KangLin/RabbitCommon ag:")
    message("   git clone https://github.com/KangLin/RabbitCommon.git")
    error  ("2. Then set value RabbitCommon_DIR to download root dirctory")
}

# Default rules for deployment.
isEmpty(PREFIX) {
    qnx: PREFIX = /tmp/$${TARGET}
    else: android: PREFIX = /.
    else: unix: PREFIX = /opt/$${TARGET}
    else: PREFIX = $$OUT_PWD/../install
}

target.path = $$PREFIX/bin
INSTALLS += target

install_win.files = Install/Install.nsi Resource/Image/App.ico #windows install need
install_win.path = $$OUT_PWD
install_win.CONFIG += directory no_check_exist 
win32:  INSTALLS += install_win
