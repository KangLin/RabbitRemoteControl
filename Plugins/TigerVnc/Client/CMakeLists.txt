# Author: Kang Lin <kl222@126.com>

project(TigerVNC
    DESCRIPTION "TigerVNC client plugin"
    )

SET(TigerVNC_SRC_LIBS tigervnc Client)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets)
if(Qt${QT_VERSION_MAJOR}_FOUND)
    list(APPEND TigerVNC_SRC_LIBS Qt${QT_VERSION_MAJOR}::Widgets)
endif()

SET(TigerVNC_SOURCE_FILES
    PluginTigerVnc.cpp
    ParameterVnc.cpp
    ConnecterVnc.cpp
    ConnectVnc.cpp
    FramePixelBuffer.cpp
    DlgSettingsVnc.cpp
    DlgGetUserPasswordVNC.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../InStreamChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../OutStreamChannel.cpp
    )
SET(TigerVNC_HEADER_FILES
    PluginTigerVnc.h
    ParameterVnc.h
    ConnecterVnc.h
    ConnectVnc.h
    FramePixelBuffer.h   
    DlgSettingsVnc.h
    DlgGetUserPasswordVNC.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../InStreamChannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../OutStreamChannel.h
    )
SET(TigerVNC_UI_FILES
    DlgSettingsVnc.ui
    DlgGetUserPasswordVNC.ui
    )

ADD_PLUGIN_TARGET(NAME PluginClient${PROJECT_NAME}
    ISPLUGIN
    VERSION ${RabbitRemoteControl_VERSION}
    SOURCE_FILES ${TigerVNC_SOURCE_FILES} ${TigerVNC_HEADER_FILES}
        ${TigerVNC_UI_FILES} ${TigerVNC_RCC_FILES}
    PRIVATE_LIBS ${TigerVNC_SRC_LIBS}
    PRIVATE_INCLUDE_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    PRIVATE_DEFINITIONS ${TIGERVNC_DEFINITIONS}
    INSTALL_DIR plugins/Client
    INSTALL_RPATH ${INSTALL_RPATH}
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins/Client
    )
