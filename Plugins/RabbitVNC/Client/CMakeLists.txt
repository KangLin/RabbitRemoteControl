# Author: Kang Lin <kl222@126.com>

project(RabbitVNC)

SET(RabbitVNC_SRC_LIBS Channel RabbitVNC Client)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets)
if(Qt${QT_VERSION_MAJOR}_FOUND)
    list(APPEND RabbitVNC_SRC_LIBS Qt${QT_VERSION_MAJOR}::Widgets)
endif()

SET(RabbitVNC_SOURCE_FILES
    ParameterRabbitVNC.cpp
    ConnectRabbitVNC.cpp
    FramePixelBuffer.cpp
    ConnecterRabbitVNC.cpp
    PluginRabbitVNC.cpp
    DlgSettingsRabbitVNC.cpp
    DlgGetPasswordRabbitVNC.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../InStreamChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../OutStreamChannel.cpp
    )
SET(RabbitVNC_HEADER_FILES
    ParameterRabbitVNC.h
    ConnectRabbitVNC.h
    FramePixelBuffer.h
    ConnecterRabbitVNC.h
    PluginRabbitVNC.h
    DlgSettingsRabbitVNC.h
    DlgGetPasswordRabbitVNC.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../InStreamChannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../OutStreamChannel.h
    )
SET(RabbitVNC_UI_FILES
    DlgSettingsRabbitVNC.ui
    DlgGetPasswordRabbitVNC.ui
    )

#message("RabbitVNC links libraries:${RabbitVNC_SRC_LIBS}")
ADD_PLUGIN_TARGET(NAME PluginClient${PROJECT_NAME}
    ISPLUGIN
    VERSION ${RabbitRemoteControl_VERSION}
    SOURCE_FILES ${RabbitVNC_SOURCE_FILES} ${RabbitVNC_HEADER_FILES}
        ${RabbitVNC_UI_FILES} ${RabbitVNC_RCC_FILES}
    PRIVATE_LIBS ${RabbitVNC_SRC_LIBS}
    PRIVATE_INCLUDE_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    INSTALL_DIR plugins/Client
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins/Client
    )
