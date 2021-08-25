# Author: Kang Lin <kl222@126.com>

project(PluginServiceTigerVnc)

if(NOT TARGET Service)
    return()
endif()
SET(SRC_LIBS tigervnc Service)
SET(SOURCE_FILES
    ParameterServiceTigerVNC.cpp
    PluginServiceTigerVNC.cpp
    ServiceTigerVNC.cpp
    Connection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../Channel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../InStreamChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../OutStreamChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../QSocketOutStream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../QSocketInStream.cpp
    )
SET(HEADER_FILES
    ParameterServiceTigerVNC.h
    PluginServiceTigerVNC.h
    ServiceTigerVNC.h
    Connection.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../Channel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../InStreamChannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../OutStreamChannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../QSocketOutStream.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../QSocketInStream.h
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
    PRIVATE_INCLUDE_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    )
