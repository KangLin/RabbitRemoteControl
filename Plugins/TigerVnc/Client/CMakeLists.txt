# Author: Kang Lin <kl222@126.com>

project(TigerVNC
    DESCRIPTION "TigerVNC client plugin"
    )

SET(TigerVNC_SRC_LIBS tigervnc Plugin)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets)
if(Qt${QT_VERSION_MAJOR}_FOUND)
    list(APPEND TigerVNC_SRC_LIBS Qt${QT_VERSION_MAJOR}::Widgets)
endif()

SET(TigerVNC_SOURCE_FILES
    PluginTigerVnc.cpp
    ParameterVnc.cpp
    OperateVnc.cpp
    BackendVnc.cpp
    FramePixelBuffer.cpp
    DlgSettingsVnc.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/../InStreamChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../OutStreamChannel.cpp
    )
SET(TigerVNC_HEADER_FILES
    PluginTigerVnc.h
    ParameterVnc.h
    OperateVnc.h
    BackendVnc.h
    FramePixelBuffer.h   
    DlgSettingsVnc.h
    
    ${CMAKE_CURRENT_SOURCE_DIR}/../InStreamChannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../OutStreamChannel.h
    )
SET(TigerVNC_UI_FILES
    DlgSettingsVnc.ui
    
    )

list(APPEND TIGERVNC_DEFINITIONS
    PluginTigerVNC_VERSION="${RabbitRemoteControl_VERSION}")

ADD_PLUGIN_TARGET(NAME PluginClient${PROJECT_NAME}
    ISPLUGIN
    VERSION ${RabbitRemoteControl_VERSION}
    SOURCE_FILES ${TigerVNC_SOURCE_FILES} ${TigerVNC_HEADER_FILES}
        ${TigerVNC_UI_FILES} ${TigerVNC_RCC_FILES}
    PRIVATE_LIBS ${TigerVNC_SRC_LIBS}
    PRIVATE_INCLUDE_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    PRIVATE_DEFINITIONS ${TIGERVNC_DEFINITIONS}
    INSTALL_DIR plugins
    INSTALL_RPATH ${INSTALL_RPATH}
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins
    )
