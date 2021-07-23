# author: Kang Lin <kl222@126.com>

find_package(qtermwidget5)
if(NOT qtermwidget5_FOUND)
    return()
endif()

# Terminal
project(Terminal LANGUAGES CXX)

set(INSTALL_HEADER_FILES
    ConnecterPluginsTerminal.h
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
    ConnecterPluginsTerminal.cpp
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

SET(SRC_LIBS RabbitCommon qtermwidget5 Viewer ${QT_LIBRARIES})
# Translations
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

string(TOLOWER ${PROJECT_NAME} LOWER_PROJECT_NAME)
set(INSTALL_HEADER_FILES ${INSTALL_HEADER_FILES}
    ${CMAKE_CURRENT_BINARY_DIR}/${LOWER_PROJECT_NAME}_export.h)

ADD_TARGET(NAME ${PROJECT_NAME}
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${SOURCE_UI_FILES} ${RCC_FILES}
    INCLUDE_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    INSTALL_HEADER_FILES ${INSTALL_HEADER_FILES}
    LIBS ${SRC_LIBS})

GENERATE_EXPORT_HEADER(${PROJECT_NAME})
file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${LOWER_PROJECT_NAME}_export.h
    DESTINATION ${CMAKE_BINARY_DIR})

