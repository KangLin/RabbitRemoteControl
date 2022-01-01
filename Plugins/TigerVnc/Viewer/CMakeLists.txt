# Author: Kang Lin <kl222@126.com>

project(TigerVNC)
set(CMAKE_CXX_STANDARD 17)
SET(SRC_LIBS tigervnc Viewer)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network)
if(Qt${QT_VERSION_MAJOR}_FOUND)
    list(APPEND SRC_LIBS Qt${QT_VERSION_MAJOR}::Network)
endif()
SET(SOURCE_FILES
    ParameterTigerVnc.cpp
    ConnectTigerVnc.cpp
    FramePixelBuffer.cpp
    ConnecterTigerVnc.cpp
    PluginTigerVnc.cpp
    DlgSettingsTigerVnc.cpp
    DlgGetUserPassword.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../Channel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../InStreamChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../OutStreamChannel.cpp
    )
SET(HEADER_FILES
    ParameterTigerVnc.h
    ConnectTigerVnc.h
    FramePixelBuffer.h
    ConnecterTigerVnc.h
    PluginTigerVnc.h
    DlgSettingsTigerVnc.h
    DlgGetUserPassword.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../Channel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../InStreamChannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../OutStreamChannel.h
    )
SET(UI_FILES
    DlgSettingsTigerVnc.ui
    DlgGetUserPassword.ui
    )

find_package(LibDataChannel)
if(LibDataChannel_FOUND)
    list(APPEND TIGERVNC_DEFINITIONS HAVE_ICE)
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
        list(APPEND TIGERVNC_DEFINITIONS HAVE_QXMPP)
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
    PRIVATE_DEFINITIONS ${TIGERVNC_DEFINITIONS}
    INSTALL_DIR plugins/Viewer
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins/Viewer
    )
