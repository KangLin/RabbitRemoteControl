# Author: Kang Lin <kl222@126.com>

project(PluginServiceLibVNCServer)

find_package(LibVNCServer)
if(LibVNCServer_FOUND)
    list(APPEND SRC_LIBS LibVNCServer::vncserver)
    message("LibVNCServer: ${LibVNCServer_FOUND}")
else()
    find_path(LibvncIncludeDir NAMES rfb/rfb.h
        HINTS ${LibVNCServer_DIR}
        PATHS ENV LibVNCServer_DIR
        PATH_SUFFIXES include
        )
    message("LibvncIncludeDir:${LibvncIncludeDir}")
    find_library(libvncLibs NAMES vncserver
        HINTS ${LibVNCServer_DIR}
        PATHS ENV LibVNCServer_DIR
        PATH_SUFFIXES lib
        )
    if(NOT (LibvncIncludeDir AND libvncLibs))
        message("Install libvnc and set LibVNCServer_DIR variable to install root directory")
        return()
    endif()
    set(_LibvncIncludeDir ${LibvncIncludeDir})
    list(APPEND SRC_LIBS ${libvncLibs})
    mark_as_advanced(LibVNCServer_DIR)
    message("libvnc found: ${libvncLibs}")
endif()

list(APPEND SRC_LIBS RabbitRemoteControlService)
SET(SOURCE_FILES
    PluginServiceLibVNCServer.cpp
    ServiceLibVNCServer.cpp
    )
SET(HEADER_FILES
    PluginServiceLibVNCServer.h
    ServiceLibVNCServer.h
    )
SET(UI_FILES
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

if(_LibvncIncludeDir)
    ADD_PLUGIN_TARGET(NAME ${PROJECT_NAME}
        ISPLUGIN
        SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
        PRIVATE_LIBS ${SRC_LIBS}
        PRIVATE_INCLUDE_DIRS ${_LibvncIncludeDir}
        )
else()
    ADD_PLUGIN_TARGET(NAME ${PROJECT_NAME}
        ISPLUGIN
        SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
        PRIVATE_LIBS ${SRC_LIBS}
        )
endif()
