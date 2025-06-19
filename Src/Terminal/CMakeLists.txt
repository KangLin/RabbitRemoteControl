# author: Kang Lin <kl222@126.com>

find_package(qtermwidget6)
if(NOT qtermwidget6_FOUND)
    return()
endif()

# Terminal
project(Terminal LANGUAGES CXX)

set(INSTALL_HEADER_FILES
    OperateTerminal.h
    FrmParameterTerminalAppearanceSettings.h
    FrmParameterTerminalBehavior.h
    ParameterTerminal.h
    
    )

set(HEADER_FILES
    ${INSTALL_HEADER_FILES}
    )

set(SOURCE_FILES
    OperateTerminal.cpp
    FrmParameterTerminalAppearanceSettings.cpp
    FrmParameterTerminalBehavior.cpp
    ParameterTerminal.cpp
    
    )

set(SOURCE_UI_FILES
    FrmParameterTerminalAppearanceSettings.ui
    FrmParameterTerminalBehavior.ui
    )

SET(SRC_LIBS RabbitCommon qtermwidget6 Plugin ${QT_LIBRARIES})

ADD_TARGET(NAME ${PROJECT_NAME}
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${SOURCE_UI_FILES} ${RCC_FILES}
    INCLUDE_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    INSTALL_HEADER_FILES ${INSTALL_HEADER_FILES}
    LIBS ${SRC_LIBS}
    VERSION ${RabbitRemoteControl_VERSION})
