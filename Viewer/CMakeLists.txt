# Author: Kang Lin(kl222@126.com)

cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "verbose")

project(Viewer LANGUAGES CXX)

set(INSTALL_HEADER_FILES
    FrmViewer.h
    Connecter.h
    ManageConnecter.h
    ManageConnect.h
    PluginViewer.h
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

set(SOURCE_FILES
    FrmViewer.cpp
    Connecter.cpp
    Connect.cpp
    ManageConnecter.cpp
    ManageConnect.cpp
    PluginViewer.cpp
    ConnecterDesktop.cpp
    ConnectThread.cpp
    PluginThread.cpp
    Parameter.cpp
    ManagePassword.cpp
    DlgInputPassword.cpp
    )

set(SOURCE_UI_FILES
    FrmViewer.ui
    DlgInputPassword.ui
    )

SET(RCC_FILES )

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
    INSTALL_EXPORT_NAME ${PROJECT_NAME}Targets
    INSTALL_NAMESPACE RabbitRemoteControl
    )

option(BUILD_TERMINAL "Build terminal plugins" ON)
if(BUILD_TERMINAL)
    add_subdirectory(Terminal)
endif(BUILD_TERMINAL)
