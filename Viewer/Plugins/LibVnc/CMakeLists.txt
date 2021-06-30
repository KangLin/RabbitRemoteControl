# Author: Kang Lin(kl222@126.com)

project(LibVnc)

set(_libvncLibs RabbitRemoteControl)

find_package(libvncclient)
if(vncclient_FOUND)
    list(APPEND _libvncLibs vncclient)
    message("vncclient: ${vncclient_FOUND}")
else()
    find_path(LibvncIncludeDir NAMES rfb/rfbclient.h
        HINTS ${vncclient_DIR}
        PATHS ENV vncclient_DIR
        PATH_SUFFIXES include
        )
    message("LibvncIncludeDir:${LibvncIncludeDir}")
    find_library(libvncLibs NAMES vncclient
        HINTS ${vncclient_DIR}
        PATHS ENV vncclient_DIR
        PATH_SUFFIXES lib
        )
    if(NOT (LibvncIncludeDir AND libvncLibs))
        message("Install libvnc and set vncclient_DIR variable to install root directory")
        return()
    endif()
    set(_LibvncIncludeDir ${LibvncIncludeDir})
    list(APPEND _libvncLibs ${libvncLibs})
    mark_as_advanced(vncclient_DIR)
    message("libvnc found: ${_libvncLibs}")
endif()

IF(WIN32)
    list(APPEND _libvncLibs ws2_32)
ENDIF()

find_package(ZLIB)
if(ZLIB_FOUND)
    list(APPEND _LibvncIncludeDir ${ZLIB_INCLUDE_DIRS})
    list(APPEND _libvncLibs ${ZLIB_LIBRARIES})
else()
    message("Please set ZLIB_ROOT variable")
endif()

find_package(OpenSSL)
if(OpenSSL_FOUND)
    list(APPEND _libvncLibs OpenSSL::SSL OpenSSL::Crypto)
endif()

SET(SOURCE_FILES
    PluginFactoryLibVnc.cpp
    ConnecterLibVnc.cpp
    ConnectLibVnc.cpp
    DlgSettingsLibVnc.cpp
    )

SET(HEADER_FILES
    PluginFactoryLibVnc.h
    ConnecterLibVnc.h
    ConnectLibVnc.h
    DlgSettingsLibVnc.h
    )

SET(UI_FILES
    DlgSettingsLibVnc.ui
    )

# Translations
GENERATED_QT_TRANSLATIONS(
    SOURCES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES}
    OUT_QRC FREERDP_TRANSLATIONS_QRC_FILES
    )
if(CMAKE_BUILD_TYPE)
    string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
endif()
if(BUILD_TYPE STREQUAL "debug")
    LIST(APPEND PRIVATE_DEFINITIONS DEBUG)
    LIST(APPEND RCC_FILES
        ${FREERDP_TRANSLATIONS_QRC_FILES}
        )
endif()

if(_LibvncIncludeDir)
    ADD_PLUGIN_TARGET(NAME ${PROJECT_NAME}
        ISPLUGIN
        SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
        PRIVATE_LIBS ${_libvncLibs}
        PRIVATE_INCLUDE_DIRS ${_LibvncIncludeDir}
        )
else()
    ADD_PLUGIN_TARGET(NAME ${PROJECT_NAME}
        ISPLUGIN
        SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
        PRIVATE_LIBS ${_libvncLibs}
        )
endif()
