# Author: Kang Lin(kl222@126.com)

find_package(qtermwidget5)
if(NOT qtermwidget5_FOUND)
    return()
endif()

project(Terminal)

list(APPEND lib_terminal qtermwidget5 RabbitRemoteControl)

SET(SOURCE_FILES
    PluginFactoryTerminal.cpp
    ConnecterTerminal.cpp
    DlgSettingsTerminal.cpp
    )

SET(HEADER_FILES
    PluginFactoryTerminal.h
    ConnecterTerminal.h
    DlgSettingsTerminal.h
    )

SET(UI_FILES
    DlgSettingsTerminal.ui
    )

#翻译
GENERATED_QT_TRANSLATIONS(
    SOURCES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES}
    OUT_QRC FREERDP_TRANSLATIONS_QRC_FILES
    )
string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
if(BUILD_TYPE STREQUAL "debug")
    LIST(APPEND PRIVATE_DEFINITIONS DEBUG)
    LIST(APPEND RCC_FILES
        ${FREERDP_TRANSLATIONS_QRC_FILES}
        )
endif()

ADD_PLUGIN_TARGET(NAME ${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
    PRIVATE_LIBS ${lib_terminal}
    )
