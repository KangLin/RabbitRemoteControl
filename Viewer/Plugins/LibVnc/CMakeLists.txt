# Author: Kang Lin(kl222@126.com)

find_path(LIBVNC_INCLUDE_DIR NAMES rfb/rfbclient.h
    HINTS ${LIBVNC_DIR}
    PATHS ENV LIBVNC_DIR
    PATH_SUFFIXES include
    )
message("LIBVNC_INCLUDE_DIR:${LIBVNC_INCLUDE_DIR}")
find_library(LIBVNC_LIB NAMES vncclient
    HINTS ${LIBVNC_DIR}
    PATHS ENV LIBVNC_DIR
    PATH_SUFFIXES lib
    )
message("LIBVNC_LIB:${LIBVNC_LIB}")
if(NOT (LIBVNC_INCLUDE_DIR AND LIBVNC_LIB))
    message("Install libvnc and set LIBVNC_DIR variable to install root directory")
	return()
endif()

mark_as_advanced(LIBVNC_DIR)
message("libvnc found: TRUE")

project(LibVnc)

list(APPEND LIBVNC_LIB RabbitRemoteControl)
IF(WIN32)
    list(APPEND LIBVNC_LIB ws2_32)
ENDIF()
message("LIBVNC_LIB:${LIBVNC_LIB}")

find_package(ZLIB)
if(ZLIB_FOUND) 
    list(APPEND LIBVNC_INCLUDE_DIR ${ZLIB_INCLUDE_DIRS})
    list(APPEND LIBVNC_LIB ${ZLIB_LIBRARIES})
else()
    message("Please set ZLIB_ROOT variable")
endif()

find_package(OPENSSL)
if(OPENSSL_FOUND)
    list(APPEND LIBVNC_LIB ${OPENSSL_CRYPTO_LIBRARIES} ${OPENSSL_LIBRARIES})
endif()

SET(SOURCE_FILES
    PluginFactoryLibVnc.cpp
    ConnecterLibVnc.cpp
    ConnectThreadLibVnc.cpp
    ConnectLibVnc.cpp
    DlgSettingsLibVnc.cpp
    )

SET(HEADER_FILES
    PluginFactoryLibVnc.h
    ConnecterLibVnc.h
    ConnectThreadLibVnc.h
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

ADD_PLUGIN_TARGET(NAME ${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
    PRIVATE_LIBS ${LIBVNC_LIB}
    PRIVATE_INCLUDE_DIRS ${LIBVNC_INCLUDE_DIR}
    )
