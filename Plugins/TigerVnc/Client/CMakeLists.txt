# Author: Kang Lin <kl222@126.com>

project(TigerVNC)

SET(TigerVNC_SRC_LIBS tigervnc Client)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets)
if(Qt${QT_VERSION_MAJOR}_FOUND)
    list(APPEND TigerVNC_SRC_LIBS Qt${QT_VERSION_MAJOR}::Widgets)
endif()

SET(TigerVNC_SOURCE_FILES
    ParameterTigerVnc.cpp
    ConnectTigerVnc.cpp
    FramePixelBuffer.cpp
    ConnecterTigerVnc.cpp
    PluginTigerVnc.cpp
    DlgSettingsTigerVnc.cpp
    DlgGetUserPasswordTigerVNC.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../RabbitVNC/InStreamChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../RabbitVNC/OutStreamChannel.cpp
    )
SET(TigerVNC_HEADER_FILES
    ParameterTigerVnc.h
    ConnectTigerVnc.h
    FramePixelBuffer.h
    ConnecterTigerVnc.h
    PluginTigerVnc.h
    DlgSettingsTigerVnc.h
    DlgGetUserPasswordTigerVNC.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../RabbitVNC/InStreamChannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../RabbitVNC/OutStreamChannel.h
    )
SET(TigerVNC_UI_FILES
    DlgSettingsTigerVnc.ui
    DlgGetUserPasswordTigerVNC.ui
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
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins/Client
    )
