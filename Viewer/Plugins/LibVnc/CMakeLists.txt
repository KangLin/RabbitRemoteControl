# Author: Kang Lin(kl222@126.com)

find_path(LIBVNC_INCLUDE_DIR NAMES rfbclient.h rfb.h
    HINTS ${LIBVNC_DIR}
    PATHS ENV LIBVNC_DIR
    PATH_SUFFIXES include/rfb
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
message("LIBVNC_LIB:${LIBVNC_LIB}")

SET(SOURCE_FILES
    PluginFactoryLibVnc.cpp
    ConnecterLibVnc.cpp
    )

SET(HEADER_FILES
    PluginFactoryLibVnc.h
    ConnecterLibVnc.h
    )

SET(UI_FILES
    
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
