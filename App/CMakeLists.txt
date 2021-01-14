#Author: Kang Lin(kl222@126.com)

cmake_minimum_required(VERSION 3.5)

project(RabbitRemoteControlApp LANGUAGES CXX)

set(SOURCE_FILES
    main.cpp
    mainwindow.cpp
    FrmFullScreenToolBar.cpp
    ViewTable.cpp
    View.cpp
    )

set(HEADER_FILES
    mainwindow.h
    FrmFullScreenToolBar.h
    ViewTable.h
    View.h
    )

set(SOURCE_UI_FILES
    mainwindow.ui
    FrmFullScreenToolBar.ui
    )

if(WIN32)
    set(RCC_FILES Resource/App.rc)
endif()
LIST(APPEND RCC_FILES Resource/Resource.qrc)

#翻译
GENERATED_QT_TRANSLATIONS(
    SOURCES ${SOURCE_FILES} ${HEADER_FILES} ${SOURCE_UI_FILES}
    OUT_QRC TRANSLATIONS_QRC_FILES
    )
string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
if(BUILD_TYPE STREQUAL "debug")
    LIST(APPEND PRIVATE_DEFINITIONS DEBUG)
    LIST(APPEND RCC_FILES
        ${TRANSLATIONS_QRC_FILES}
        )
endif()

ADD_TARGET(NAME ${PROJECT_NAME}
    ISEXE
    ISWINDOWS
    VERSION ${VERSION}
    PRIVATE_DEFINITIONS BUILD_VERSION="${BUILD_VERSION}"
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${SOURCE_UI_FILES} ${RCC_FILES}
    PRIVATE_LIBS RabbitRemoteControl)

if(WIN32)
    install(FILES Resource/Image/App.ico
        DESTINATION .
            COMPONENT Runtime)
endif()
if(UNIX AND NOT ANDROID)
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/debian/RabbitRemoteControl.desktop
        DESTINATION share/applications
            COMPONENT Runtime)
    INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Resource/Image/App.png
        DESTINATION share/pixmaps
            COMPONENT Runtime
        RENAME RabbitRemoteControl.png)
endif()
