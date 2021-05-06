# Author: Kang Lin(kl222@126.com)

find_package(vncclient)
if(vncclient_FOUND)
    set(LIBVNC_LIB vncclient)
else()
    find_path(LIBVNC_INCLUDE_DIR NAMES rfb/rfbclient.h
        HINTS ${vncclient_DIR}
        PATHS ENV vncclient_DIR
        PATH_SUFFIXES include
        )
    message("LIBVNC_INCLUDE_DIR:${LIBVNC_INCLUDE_DIR}")
    find_library(LIBVNC_LIB NAMES vncclient
        HINTS ${vncclient_DIR}
        PATHS ENV vncclient_DIR
        PATH_SUFFIXES lib
        )
    message("LIBVNC_LIB:${LIBVNC_LIB}")
    if(NOT (LIBVNC_INCLUDE_DIR AND LIBVNC_LIB))
        message("Install libvnc and set vncclient_DIR variable to install root directory")
        return()
    endif()
    set(_LIBVNC_INCLUDE_DIR ${LIBVNC_INCLUDE_DIR})
    mark_as_advanced(vncclient_DIR)
endif()
message("vncclient: ${vncclient_FOUND}")
message("libvnc found: ${LIBVNC_LIB}")

project(LibVnc)

list(APPEND LIBVNC_LIB RabbitRemoteControl)
IF(WIN32)
    list(APPEND LIBVNC_LIB ws2_32)
ENDIF()
message("LIBVNC_LIB:${LIBVNC_LIB}")
message("LIBVNC_INCLUDE_DIR:${_LIBVNC_INCLUDE_DIR}")
find_package(ZLIB)
if(ZLIB_FOUND)
    list(APPEND _LIBVNC_INCLUDE_DIR ${ZLIB_INCLUDE_DIRS})
    list(APPEND LIBVNC_LIB ${ZLIB_LIBRARIES})
else()
    message("Please set ZLIB_ROOT variable")
endif()

find_package(OpenSSL)
if(OpenSSL_FOUND)
    list(APPEND LIBVNC_LIB OpenSSL::SSL OpenSSL::Crypto)
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

message("LIBVNC_INCLUDE_DIR:${_LIBVNC_INCLUDE_DIR}")
if(_LIBVNC_INCLUDE_DIR)
    ADD_PLUGIN_TARGET(NAME ${PROJECT_NAME}
        ISPLUGIN
        SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
        PRIVATE_LIBS ${LIBVNC_LIB}
        PRIVATE_INCLUDE_DIRS ${_LIBVNC_INCLUDE_DIR}
        )
else()
    ADD_PLUGIN_TARGET(NAME ${PROJECT_NAME}
        ISPLUGIN
        SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
        PRIVATE_LIBS ${LIBVNC_LIB}
        )
endif()
