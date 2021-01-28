# Author: Kang Lin(kl222@126.com)

find_package(libssh)
if(NOT libssh_FOUND)
    return()
endif()

project(LibSSH)

set(lib_ssh RabbitRemoteControlTerminal ssh)

SET(SOURCE_FILES
    PluginFactoryLibSSH.cpp
    ConnecterSSH.cpp
    DlgSettingsSSH.cpp
    ParameterSSH.cpp
    ConnectSSH.cpp
    )

SET(HEADER_FILES
    PluginFactoryLibSSH.h
    ConnecterSSH.h
    DlgSettingsSSH.h
    ParameterSSH.h
    ConnectSSH.h
    )

SET(UI_FILES
    DlgSettingsSSH.ui
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
    PRIVATE_LIBS ${lib_ssh}
    )
