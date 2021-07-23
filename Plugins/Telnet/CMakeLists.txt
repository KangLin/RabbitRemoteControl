# Author: Kang Lin(kl222@126.com)

project(Telnet)

set(_telnet_libs Terminal)

find_package(libtelnet)
if(libtelnet_FOUND)
    list(APPEND _telnet_libs libtelnet)
else()
    find_path(libtelnetIncludeDir NAMES libtelnet.h
        HINTS ${libtelnet_DIR}
        PATHS ENV llibtelnet_DIR
        PATH_SUFFIXES include
        )
    find_library(libtelnet_lib NAMES telnet
        HINTS ${libtelnet_DIR}
        PATHS ENV libtelnet_DIR
        PATH_SUFFIXES lib
        )
    if(NOT (libtelnetIncludeDir AND libtelnet_lib))
        message("Install libssh and set libtelnet_DIR variable to install root directory")
        return()
    endif()

    list(APPEND _telnet_libs ${libtelnet_lib})
    set(_libtelnet_include_dir ${libtelnetIncludeDir})
    mark_as_advanced(libssh_DIR)
endif()
message("libtelnet:${_telnet_libs}")

SET(SOURCE_FILES
    PluginTelnet.cpp
    ConnecterTelnet.cpp
    ParameterTelnet.cpp
    DlgSettingsTelnet.cpp
    )

SET(HEADER_FILES
    ConnecterTelnet.h
    ParameterTelnet.h
    PluginTelnet.h
    DlgSettingsTelnet.h
    )

SET(UI_FILES
    DlgSettingsTelnet.ui
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

if(_libtelnet_include_dir)
    ADD_PLUGIN_TARGET(NAME ${PROJECT_NAME}
        ISPLUGIN
        SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
        PRIVATE_LIBS ${_telnet_libs}
        PRIVATE_INCLUDE_DIRS ${_libtelnet_include_dir}
        )
else()
    ADD_PLUGIN_TARGET(NAME ${PROJECT_NAME}
        ISPLUGIN
        SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
        PRIVATE_LIBS ${_telnet_libs}
        )
endif()
