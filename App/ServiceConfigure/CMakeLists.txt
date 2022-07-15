# Author: Kang Lin <kl222@126.com>

cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "verbose")

project(RabbitRemoteControlServiceConfigure LANGUAGES CXX)

if(WIN32)
    set(RCC_FILES Resource/App.rc)
endif()

find_package(QtService)
if(NOT QtService_FOUND)
    return()
endif()
message("QtServeice is found")
SET(SRC_LIBS RabbitCommon ${QT_LIBRARIES})

list(APPEND SRC_LIBS Service)

set(HEADER_FILES 
    MainWindow.h
    )
set(SOURCE_FILES
    MainWindow.cpp
    main.cpp
    )
set(SOURCE_UI_FILES MainWindow.ui)
LIST(APPEND RCC_FILES ../Viewer/Resource/Resource.qrc)

SET(QT_COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${QT_COMPONENTS})
    list(APPEND QT_LIBRARIES Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
endif()
message("${PROJECT_NAME} QT_LIBRARIES: ${QT_LIBRARIES}")
list(APPEND SRC_LIBS ${QT_LIBRARIES})

ADD_TARGET(NAME ${PROJECT_NAME}
    ISEXE
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${SOURCE_UI_FILES} ${RCC_FILES}
    PRIVATE_DEFINITIONS RabbitRemoteControl_VERSION="${RabbitRemoteControl_VERSION}"
    INCLUDE_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # See: http://www.it1352.com/478094.html
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE_LIBS ${SRC_LIBS}
    VERSION ${RabbitRemoteControl_VERSION})

if(UNIX AND NOT ANDROID)
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/share/applications/RabbitRemoteControlServiceConfigure.desktop
        DESTINATION share/applications
        COMPONENT Runtime)
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/App/Viewer/Resource/Image/option.png
        DESTINATION share/pixmaps
        COMPONENT Runtime
        RENAME RabbitRemoteControlServiceConfigure.png)
endif()

