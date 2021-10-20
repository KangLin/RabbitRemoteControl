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

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif(NOT DEFINED CMAKE_BUILD_TYPE)
if(CMAKE_BUILD_TYPE)
    string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
endif()
if(BUILD_TYPE STREQUAL "debug")
    LIST(APPEND PRIVATE_DEFINITIONS DEBUG _DEBUG)
endif()

ADD_PLUGIN_TARGET(NAME PluginViewer${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
    PRIVATE_LIBS ${lib_terminal}
    )
