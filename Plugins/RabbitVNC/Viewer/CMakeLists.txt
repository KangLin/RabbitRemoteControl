# Author: Kang Lin <kl222@126.com>

project(RabbitVNC)
set(CMAKE_CXX_STANDARD 17)

SET(SRC_LIBS RabbitVNC Viewer)
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../Channel.cpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../Channel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../InStreamChannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../OutStreamChannel.h
    )
SET(UI_FILES
    DlgSettingsRabbitVNC.ui
    DlgGetPasswordRabbitVNC.ui
    )

find_package(LibDataChannel)
if(LibDataChannel_FOUND)
    list(APPEND RABBITVNC_DEFINITIONS HAVE_ICE)
    if (TARGET LibDataChannel::LibDataChannel)
        list(APPEND SRC_LIBS LibDataChannel::LibDataChannel)
    elseif (TARGET LibDatachannel::datachannel)
        list(APPEND SRC_LIBS LibDatachannel::datachannel)
    else()
        list(APPEND SRC_LIBS LibDataChannel::datachannel)
    endif()
    list(APPEND SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../ICE/DataChannelIce.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../ICE/IceSignal.cpp)
    list(APPEND HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../ICE/DataChannelIce.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../ICE/IceSignal.h)
    if(UNIX)
        install_targets(TARGETS LibDataChannel::datachannel)
    endif()
    
    find_package(QXmpp)
    if(QXmpp_FOUND)
        list(APPEND RABBITVNC_DEFINITIONS HAVE_QXMPP)
        find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network Xml)
        list(APPEND SRC_LIBS QXmpp::QXmpp)
        install_targets(TARGETS QXmpp::QXmpp)
        list(APPEND SOURCE_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/../ICE/IceSignalQXmppIq.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/../ICE/IceSignalQXmppManager.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/../ICE/IceSignalQxmpp.cpp
            )
        list(APPEND HEADER_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/../ICE/IceSignalQXmppIq.h
            ${CMAKE_CURRENT_SOURCE_DIR}/../ICE/IceSignalQXmppManager.h
            ${CMAKE_CURRENT_SOURCE_DIR}/../ICE/IceSignalQxmpp.h
            )
    endif()
endif()

ADD_PLUGIN_TARGET(NAME PluginViewer${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
    PRIVATE_LIBS ${SRC_LIBS}
    PRIVATE_INCLUDE_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    PRIVATE_DEFINITIONS ${RABBITVNC_DEFINITIONS}
    INSTALL_DIR plugins/Viewer
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins/Viewer
    )
