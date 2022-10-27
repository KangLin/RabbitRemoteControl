# Author: Kang Lin <kl222@126.com>

cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "verbose")

project(RabbitRemoteControlServiceConfigure LANGUAGES CXX)

if(WIN32)
    set(RabbitRemoteControlServiceConfigure_RCC_FILES Resource/App.rc)
endif()

find_package(QtService)
if(NOT QtService_FOUND)
    return()
endif()
message("QtServeice is found")
SET(RabbitRemoteControlServiceConfigure_SRC_LIBS RabbitCommon)

list(APPEND RabbitRemoteControlServiceConfigure_SRC_LIBS Service)

set(HEADER_FILES 
    MainWindow.h
    )
set(SOURCE_FILES
    MainWindow.cpp
    main.cpp
    )
set(SOURCE_UI_FILES MainWindow.ui)
LIST(APPEND RabbitRemoteControlServiceConfigure_RCC_FILES ../Client/Resource/Resource.qrc)

SET(QT_COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${QT_COMPONENTS})
    list(APPEND RabbitRemoteControlServiceConfigure_QT_LIBRARIES Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
endif()
message("${PROJECT_NAME} RabbitRemoteControlServiceConfigure_QT_LIBRARIES: ${RabbitRemoteControlServiceConfigure_QT_LIBRARIES}")
list(APPEND RabbitRemoteControlServiceConfigure_SRC_LIBS ${RabbitRemoteControlServiceConfigure_QT_LIBRARIES})

ADD_TARGET(NAME ${PROJECT_NAME}
    ISEXE
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${SOURCE_UI_FILES} ${RabbitRemoteControlServiceConfigure_RCC_FILES}
    PRIVATE_DEFINITIONS RabbitRemoteControl_VERSION="${RabbitRemoteControl_VERSION}"
    INCLUDE_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # See: http://www.it1352.com/478094.html
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE_LIBS ${RabbitRemoteControlServiceConfigure_SRC_LIBS}
    VERSION ${RabbitRemoteControl_VERSION})

if(UNIX AND NOT ANDROID)
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/share/applications/RabbitRemoteControlServiceConfigure.desktop
        DESTINATION share/applications
            COMPONENT Runtime)
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/App/Client/Resource/Image/system-settings.svg
        DESTINATION share/pixmaps
            COMPONENT Runtime
        RENAME RabbitRemoteControlServiceConfigure.svg)
endif()

configure_file(${CMAKE_SOURCE_DIR}/etc/RabbitRemoteControl.conf
    ${CMAKE_BINARY_DIR}/etc/${PROJECT_NAME}.conf
    COPYONLY)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/etc/RabbitRemoteControl.conf
    DESTINATION etc
        COMPONENT Runtime
    RENAME ${PROJECT_NAME}.conf)
