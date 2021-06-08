#Author: Kang Lin(kl222@126.com)

project(TigerVnc)

find_package(tigervnc)
message("tigervnc_FOUND:${tigervnc_FOUND}")
if(NOT tigervnc_FOUND)
    return()
endif()

SET(SRC_LIBS tigervnc RabbitRemoteControl)
SET(SOURCE_FILES
    ConnectTigerVnc.cpp
    FramePixelBuffer.cpp
    ConnecterTigerVnc.cpp
    PluginFactoryTigerVnc.cpp
    DlgSettingsTigerVnc.cpp
    QSocketInStream.cpp
    QSocketOutStream.cpp
    )
SET(HEADER_FILES
    ConnectTigerVnc.h
    FramePixelBuffer.h
    ConnecterTigerVnc.h
    PluginFactoryTigerVnc.h
    DlgSettingsTigerVnc.h
    QSocketInStream.h
    QSocketOutStream.h
    )
SET(UI_FILES
    DlgSettingsTigerVnc.ui
    )

#翻译
GENERATED_QT_TRANSLATIONS(
    SOURCES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES}
    OUT_QRC TIGER_VNC_TRANSLATIONS_QRC_FILES
    )
if(CMAKE_BUILD_TYPE)
    string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
endif()
if(BUILD_TYPE STREQUAL "debug")
    LIST(APPEND PRIVATE_DEFINITIONS DEBUG)
    LIST(APPEND RCC_FILES
        ${TIGER_VNC_TRANSLATIONS_QRC_FILES}
        )
endif()

ADD_PLUGIN_TARGET(NAME ${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
    PRIVATE_LIBS ${SRC_LIBS}
    )
