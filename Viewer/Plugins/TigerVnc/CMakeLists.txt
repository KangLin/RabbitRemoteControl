
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
    ConnectThread.cpp
    ManageConnectTigerVnc.cpp
    DlgSettings.cpp
    )
SET(HEADER_FILES
    ConnectTigerVnc.h
    FramePixelBuffer.h
    ConnecterTigerVnc.h
    ConnectThread.h
    ManageConnectTigerVnc.h
    DlgSettings.h
    )
SET(UI_FILES
    DlgSettings.ui
    )

#翻译
GENERATED_QT_TRANSLATIONS(
    SOURCES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES}
    OUT_QRC TRANSLATIONS_QRC_FILES
    )
string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
if(BUILD_TYPE STREQUAL "debug")
    LIST(APPEND PRIVATE_DEFINITIONS DEBUG)
    LIST(APPEND RCC_FILES
        ${TRANSLATIONS_QRC_FILES}
        )
endif()

ADD_PLUGIN_TARGET(NAME ${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES}
    PRIVATE_LIBS ${SRC_LIBS}
    )
