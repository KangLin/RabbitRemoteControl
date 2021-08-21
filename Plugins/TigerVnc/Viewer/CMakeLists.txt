# Author: Kang Lin <kl222@126.com>

project(TigerVnc)
set(CMAKE_CXX_STANDARD 17)
SET(SRC_LIBS tigervnc Viewer)
SET(SOURCE_FILES
    ConnectTigerVnc.cpp
    FramePixelBuffer.cpp
    ConnecterTigerVnc.cpp
    PluginTigerVnc.cpp
    DlgSettingsTigerVnc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../DataChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../InStreamDataChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../OutStreamDataChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../QSocketInStream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../QSocketOutStream.cpp
    )
SET(HEADER_FILES
    ConnectTigerVnc.h
    FramePixelBuffer.h
    ConnecterTigerVnc.h
    PluginTigerVnc.h
    DlgSettingsTigerVnc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../DataChannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../InStreamDataChannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../OutStreamDataChannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../QSocketInStream.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../QSocketOutStream.h
    )
SET(UI_FILES
    DlgSettingsTigerVnc.ui
    )

find_package(libdatachannel)
if(libdatachannel_FOUND)
    list(APPEND SRC_LIBS LibDatachannel::datachannel)
    list(APPEND SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../ICE/DataChannelIce.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../ICE/IceSignal.cpp)
    list(APPEND HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../ICE/DataChannelIce.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../ICE/IceSignal.h)
endif()

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
