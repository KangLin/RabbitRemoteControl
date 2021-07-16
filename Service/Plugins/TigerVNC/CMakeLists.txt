# Author: Kang Lin <kl222@126.com>

project(PluginServiceTigerVnc)

find_package(tigervnc)
message("tigervnc_FOUND:${tigervnc_FOUND}")
if(NOT tigervnc_FOUND)
    return()
endif()

SET(SRC_LIBS tigervnc RabbitRemoteControlService)
SET(SOURCE_FILES
    PluginServiceTigerVNC.cpp
    ServiceTigerVNC.cpp
    QSocket.cpp
    )
SET(HEADER_FILES
    PluginServiceTigerVNC.h
    ServiceTigerVNC.h
    QSocket.h
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

ADD_PLUGIN_TARGET(NAME ${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
    PRIVATE_LIBS ${SRC_LIBS}
    )
