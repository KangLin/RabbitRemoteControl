# Author: Kang Lin(kl222@126.com)

project(Terminal)

set(lib_terminal Terminal)

SET(SOURCE_FILES
    PluginTerminal.cpp
    DlgSettingsTerminal.cpp
    ConnecterTerminal.cpp
    )

SET(HEADER_FILES
    PluginTerminal.h
    DlgSettingsTerminal.h
    ConnecterTerminal.h
    )

SET(UI_FILES
    DlgSettingsTerminal.ui
    )

# Translations
GENERATED_QT_TRANSLATIONS(
    SOURCES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES}
    OUT_QRC ${PROJECT_NAME}_TRANSLATIONS_QRC_FILES
    )
if(CMAKE_BUILD_TYPE)
    string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
endif()
if(BUILD_TYPE STREQUAL "debug")
    LIST(APPEND PRIVATE_DEFINITIONS DEBUG)
    LIST(APPEND RCC_FILES
        ${${PROJECT_NAME}_TRANSLATIONS_QRC_FILES}
        )
endif()

ADD_PLUGIN_TARGET(NAME PluginViewer${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
    PRIVATE_LIBS ${lib_terminal}
    )
