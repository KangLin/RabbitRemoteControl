# Author: Kang Lin(kl222@126.com)

find_package(libssh)
if(libssh_FOUND)
    set(libssh_LIB ssh)
else()
    find_path(libssh_INCLUDE_DIR NAMES libssh.h libssh_version.h
        HINTS ${libssh_DIR}
        PATHS ENV libssh_DIR
        PATH_SUFFIXES include include/libssh
        )
    message("libssh_INCLUDE_DIR:${libssh_INCLUDE_DIR}")
    find_library(libssh_LIB NAMES ssh
        HINTS ${libssh_DIR}
        PATHS ENV libssh_DIR
        PATH_SUFFIXES lib
        )
    message("libssh_LIB:${LIBVNC_LIB}")
    if(NOT (libssh_INCLUDE_DIR AND libssh_LIB))
        message("Install libssh and set libssh_DIR variable to install root directory")
        return()
    endif()

    mark_as_advanced(libssh_DIR)
endif()

project(LibSSH)

list(APPEND libssh_LIB RabbitRemoteControlTerminal)

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

if(libssh_INCLUDE_DIR)
    ADD_PLUGIN_TARGET(NAME ${PROJECT_NAME}
        ISPLUGIN
        SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
        PRIVATE_LIBS ${libssh_LIB}
        PRIVATE_INCLUDE_DIRS ${libssh_INCLUDE_DIR}
        )
else()
    ADD_PLUGIN_TARGET(NAME ${PROJECT_NAME}
        ISPLUGIN
        SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
        PRIVATE_LIBS ${libssh_LIB}
        )
endif()
