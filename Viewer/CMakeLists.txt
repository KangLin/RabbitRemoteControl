# Author: Kang Lin <kl222@126.com>

cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "verbose")

project(Viewer LANGUAGES CXX)

set(INSTALL_HEADER_FILES )
set(SOURCE_FILES )

option(USE_FROM_OPENGL "Use opengl from" OFF)
if(USE_FROM_OPENGL)
    set(VIEWER_DEFINITIONS USE_FROM_OPENGL)
    list(APPEND SOURCE_FILES
        FrmViewerOpenGL.cpp)
    list(APPEND INSTALL_HEADER_FILES
        FrmViewerOpenGL.h)
else()
    list(APPEND SOURCE_FILES
        FrmViewer.cpp)
endif()

list(APPEND INSTALL_HEADER_FILES
    FrmViewer.h
    Connecter.h
    ManagePlugin.h
    ManageConnect.h
    PluginViewer.h
    PluginViewerThread.h
    ConnecterDesktopThread.h
    ConnecterDesktop.h
    Parameter.h
    ManagePassword.h
    )

set(HEADER_FILES
    ${INSTALL_HEADER_FILES}
    Connect.h
    ConnectThread.h
    PluginThread.h
    DlgInputPassword.h
    )

list(APPEND SOURCE_FILES
    Connecter.cpp
    Connect.cpp
    ManagePlugin.cpp
    ManageConnect.cpp
    PluginViewer.cpp
    PluginViewerThread.cpp
    ConnecterDesktopThread.cpp
    ConnecterDesktop.cpp
    ConnectThread.cpp
    PluginThread.cpp
    Parameter.cpp
    ManagePassword.cpp
    DlgInputPassword.cpp
    )

set(SOURCE_UI_FILES
    DlgInputPassword.ui
    )

SET(RCC_FILES )

SET(QT_COMPONENTS Gui Widgets)
if(ANDROID)
    SET(QT_COMPONENTS ${QT_COMPONENTS} AndroidExtras)
endif()
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_COMPONENTS})
message("QT_VERSION:${Qt${QT_VERSION_MAJOR}_VERSION}")
if(Qt${QT_VERSION_MAJOR}_VERSION VERSION_LESS 5.10.0 AND ANDROID)
    message(FATAL_ERROR "Qt must great 5.10.0")
endif()
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${QT_COMPONENTS})
        list(APPEND QT_LIBRARIES Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
endif()
message("${PROJECT_NAME} QT_LIBRARIES: ${QT_LIBRARIES}")
SET(SRC_LIBS RabbitCommon ${QT_LIBRARIES})

ADD_TARGET(NAME ${PROJECT_NAME}
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${SOURCE_UI_FILES} ${RCC_FILES}
    INCLUDE_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # See: http://www.it1352.com/478094.html
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    INSTALL_HEADER_FILES ${INSTALL_HEADER_FILES}
    LIBS ${SRC_LIBS}
    VERSION ${BUILD_VERSION}
    PRIVATE_DEFINITIONS ${PRIVATE_DEFINITIONS}
    DEFINITIONS ${VIEWER_DEFINITIONS}
    INSTALL_EXPORT_NAME ${PROJECT_NAME}Targets
    INSTALL_NAMESPACE RabbitRemoteControl
    )

option(BUILD_TERMINAL "Build terminal plugins" ON)
if(BUILD_TERMINAL)
    add_subdirectory(Terminal)
endif(BUILD_TERMINAL)
