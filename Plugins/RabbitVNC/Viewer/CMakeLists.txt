# Author: Kang Lin <kl222@126.com>

project(RabbitVNC)
set(CMAKE_CXX_STANDARD 17)

SET(SRC_LIBS Channel RabbitVNC Viewer)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network)
if(Qt${QT_VERSION_MAJOR}_FOUND)
    list(APPEND SRC_LIBS Qt${QT_VERSION_MAJOR}::Network)
endif()
SET(SOURCE_FILES
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
SET(HEADER_FILES
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
SET(UI_FILES
    DlgSettingsRabbitVNC.ui
    DlgGetPasswordRabbitVNC.ui
    )

#message("RabbitVNC links libraries:${SRC_LIBS}")
ADD_PLUGIN_TARGET(NAME PluginViewer${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
    PRIVATE_LIBS ${SRC_LIBS}
    PRIVATE_INCLUDE_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    INSTALL_DIR plugins/Viewer
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins/Viewer
    )
