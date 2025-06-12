# author: Kang Lin <kl222@126.com>

find_package(qtermwidget5)
if(NOT qtermwidget5_FOUND)
    return()
endif()

# Terminal
project(Terminal LANGUAGES CXX)

set(INSTALL_HEADER_FILES
    ConnecterTerminal.h
    FrmParameterTerminalAppearanceSettings.h
    FrmParameterTerminalBehavior.h
    ParameterTerminal.h
    FrmTermWidget.h
    )

set(HEADER_FILES
    ${INSTALL_HEADER_FILES}
    ConnectThreadTerminal.h
    )

set(SOURCE_FILES
    ConnecterTerminal.cpp
    FrmParameterTerminalAppearanceSettings.cpp
    FrmParameterTerminalBehavior.cpp
    ParameterTerminal.cpp
    ConnectThreadTerminal.cpp
    FrmTermWidget.cpp
    )

set(SOURCE_UI_FILES
    FrmParameterTerminalAppearanceSettings.ui
    FrmParameterTerminalBehavior.ui
    )

SET(SRC_LIBS RabbitCommon qtermwidget5 Client ${QT_LIBRARIES})
if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif(NOT DEFINED CMAKE_BUILD_TYPE)
if(CMAKE_BUILD_TYPE)
    string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
endif()
if(BUILD_TYPE STREQUAL "debug")
    LIST(APPEND PRIVATE_DEFINITIONS DEBUG _DEBUG)
endif()

ADD_TARGET(NAME ${PROJECT_NAME}
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${SOURCE_UI_FILES} ${RCC_FILES}
    INCLUDE_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    INSTALL_HEADER_FILES ${INSTALL_HEADER_FILES}
    LIBS ${SRC_LIBS}
    VERSION ${RabbitRemoteControl_VERSION})
