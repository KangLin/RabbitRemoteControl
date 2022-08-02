# Author: Kang Lin <kl222@126.com>

cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "verbose")

project(Client LANGUAGES CXX)

option(USE_FROM_OPENGL "Use opengl from" OFF)
if(USE_FROM_OPENGL)
    set(VIEWER_DEFINITIONS USE_FROM_OPENGL)
    list(APPEND CLIENT_SOURCE_FILES
        FrmViewerOpenGL.cpp)
    list(APPEND CLIENT_INSTALL_HEADER_FILES
        FrmViewerOpenGL.h)
else()
    list(APPEND CLIENT_SOURCE_FILES
        FrmViewer.cpp)
endif()

list(APPEND CLIENT_INSTALL_HEADER_FILES
    FrmViewer.h
    Connecter.h
    Client.h
    ManageConnect.h
    PluginClient.h
    PluginClientThread.h
    ConnecterDesktopThread.h
    ConnecterDesktop.h
    Parameter.h
    ParameterConnecter.h
    ParameterClient.h
    )

list(APPEND CLIENT_HEADER_FILES
    ${CLIENT_INSTALL_HEADER_FILES}
    Connect.h
    ConnectThread.h
    PluginThread.h
    DlgInputPassword.h
    Hook.h
    FrmParameterClient.h
    )

list(APPEND CLIENT_SOURCE_FILES
    Connecter.cpp
    Connect.cpp
    Client.cpp
    ManageConnect.cpp
    PluginClient.cpp
    PluginClientThread.cpp
    ConnecterDesktopThread.cpp
    ConnecterDesktop.cpp
    ConnectThread.cpp
    PluginThread.cpp
    Parameter.cpp
    ParameterConnecter.cpp
    ParameterClient.cpp
    FrmParameterClient.cpp
    DlgInputPassword.cpp
    Hook.cpp
    )

set(CLIENT_UI_FILES
    DlgInputPassword.ui
    FrmParameterClient.ui
    )

SET(RCC_FILES )

if(WIN32)
    list(APPEND CLIENT_HEADER_FILES Windows/HookWindows.h)
    list(APPEND CLIENT_SOURCE_FILES Windows/HookWindows.cpp)
endif()

list(APPEND CLIENT_LIBS Channel)
if(UNIX)
    find_package(GTK2)
    if(GTK2_FOUND)
        list(APPEND CLIENT_LIBS GTK2::gtk GTK2::gdk)
    endif(NOT GTK3_FOUND)
endif()

SET(VIEWER_QT_COMPONENTS Core Gui Widgets)
if(ANDROID)
    SET(VIEWER_QT_COMPONENTS ${VIEWER_QT_COMPONENTS} AndroidExtras)
endif()
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${VIEWER_QT_COMPONENTS})
message("QT_VERSION:${Qt${QT_VERSION_MAJOR}_VERSION}")
if(Qt${QT_VERSION_MAJOR}_VERSION VERSION_LESS 5.10.0 AND ANDROID)
    message(FATAL_ERROR "Qt must great 5.10.0")
endif()
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${VIEWER_QT_COMPONENTS})
        list(APPEND QT_LIBRARIES Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
endif()
message("${PROJECT_NAME} QT_LIBRARIES: ${QT_LIBRARIES}")
list(APPEND CLIENT_LIBS ${QT_LIBRARIES})

ADD_TARGET(NAME ${PROJECT_NAME}
    SOURCE_FILES ${CLIENT_SOURCE_FILES} ${CLIENT_HEADER_FILES} ${CLIENT_UI_FILES} ${RCC_FILES}
    INCLUDE_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # See: http://www.it1352.com/478094.html
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    INSTALL_HEADER_FILES ${CLIENT_INSTALL_HEADER_FILES}
    LIBS ${CLIENT_LIBS}
    VERSION ${RabbitRemoteControl_VERSION}
    PRIVATE_DEFINITIONS ${PRIVATE_DEFINITIONS}
    DEFINITIONS ${VIEWER_DEFINITIONS}
    INSTALL_EXPORT_NAME ${PROJECT_NAME}Targets
    INSTALL_NAMESPACE RabbitRemoteControl
    )

option(BUILD_VIEWER_TERMINAL "Build terminal Client" ON)
if(BUILD_VIEWER_TERMINAL)
    add_subdirectory(Terminal)
endif(BUILD_VIEWER_TERMINAL)
