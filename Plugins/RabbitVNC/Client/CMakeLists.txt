# Author: Kang Lin <kl222@126.com>

project(RabbitVNC
    DESCRIPTION "RabbitVNC client plugin"
    )

SET(RabbitVNC_SRC_LIBS RabbitVNC Client)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets)
if(Qt${QT_VERSION_MAJOR}_FOUND)
    list(APPEND RabbitVNC_SRC_LIBS Qt${QT_VERSION_MAJOR}::Widgets)
endif()

SET(RabbitVNC_SOURCE_FILES
    PluginRabbitVNC.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../TigerVnc/Client/ParameterVnc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../TigerVnc/Client/ConnecterVnc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../TigerVnc/Client/ConnectVnc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../TigerVnc/Client/FramePixelBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../TigerVnc/Client/DlgSettingsVnc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../TigerVnc/Client/DlgGetUserPasswordVNC.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../TigerVnc/InStreamChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../TigerVnc/OutStreamChannel.cpp
    )
SET(RabbitVNC_HEADER_FILES
    PluginRabbitVNC.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../TigerVnc/Client/ParameterVnc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../TigerVnc/Client/ConnecterVnc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../TigerVnc/Client/ConnectVnc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../TigerVnc/Client/FramePixelBuffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../TigerVnc/Client/DlgSettingsVnc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../TigerVnc/Client/DlgGetUserPasswordVNC.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../TigerVnc/InStreamChannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../TigerVnc/OutStreamChannel.h
    )
SET(RabbitVNC_UI_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../../TigerVnc/Client/DlgSettingsVnc.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/../../TigerVnc/Client/DlgGetUserPasswordVNC.ui
    )

#message("RabbitVNC links libraries:${RabbitVNC_SRC_LIBS}")
ADD_PLUGIN_TARGET(NAME PluginClient${PROJECT_NAME}
    ISPLUGIN
    VERSION ${RabbitRemoteControl_VERSION}
    SOURCE_FILES ${RabbitVNC_SOURCE_FILES} ${RabbitVNC_HEADER_FILES}
        ${RabbitVNC_UI_FILES} ${RabbitVNC_RCC_FILES}
    PRIVATE_LIBS ${RabbitVNC_SRC_LIBS}
    PRIVATE_INCLUDE_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../TigerVnc/Client>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../TigerVnc>
    INSTALL_DIR plugins/Client
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins/Client
    )
