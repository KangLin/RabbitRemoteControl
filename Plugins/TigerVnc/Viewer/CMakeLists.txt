# Author: Kang Lin <kl222@126.com>

project(TigerVnc)

SET(SRC_LIBS tigervnc Viewer)
SET(SOURCE_FILES
    ConnectTigerVnc.cpp
    FramePixelBuffer.cpp
    ConnecterTigerVnc.cpp
    PluginTigerVnc.cpp
    DlgSettingsTigerVnc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../QSocketInStream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../QSocketOutStream.cpp
    )
SET(HEADER_FILES
    ConnectTigerVnc.h
    FramePixelBuffer.h
    ConnecterTigerVnc.h
    PluginTigerVnc.h
    DlgSettingsTigerVnc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../QSocketInStream.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../QSocketOutStream.h
    )
SET(UI_FILES
    DlgSettingsTigerVnc.ui
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

ADD_PLUGIN_TARGET(NAME PluginViewer${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
    PRIVATE_LIBS ${SRC_LIBS}
    PRIVATE_INCLUDE_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    )
