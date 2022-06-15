# Author: Kang Lin <kl222@126.com>

cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "verbose")

project(Channel LANGUAGES CXX)

list(APPEND CHANNEL_INSTALL_CHANNEL_HEADER_FILES
    Channel.h
    )

set(CHANNEL_HEADER_FILES
    ${CHANNEL_INSTALL_CHANNEL_HEADER_FILES}
    )

list(APPEND CHANNEL_SOURCE_FILES
    Channel.cpp
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
        ${CMAKE_CURRENT_SOURCE_DIR}/ICE/DataChannelIce.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ICE/IceSignal.cpp)
    list(APPEND HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/ICE/DataChannelIce.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ICE/IceSignal.h)
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
            ${CMAKE_CURRENT_SOURCE_DIR}/ICE/IceSignalQXmppIq.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ICE/IceSignalQXmppManager.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ICE/IceSignalQxmpp.cpp
            )
        list(APPEND HEADER_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/ICE/IceSignalQXmppIq.h
            ${CMAKE_CURRENT_SOURCE_DIR}/ICE/IceSignalQXmppManager.h
            ${CMAKE_CURRENT_SOURCE_DIR}/ICE/IceSignalQxmpp.h
            )
    endif()
endif()

SET(SRC_LIBS RabbitCommon)

ADD_TARGET(NAME ${PROJECT_NAME}
    SOURCE_FILES ${CHANNEL_SOURCE_FILES} ${CHANNEL_HEADER_FILES}
    INCLUDE_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # See: http://www.it1352.com/478094.html
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    INSTALL_HEADER_FILES ${CHANNEL_INSTALL_HEADER_FILES}
    LIBS ${SRC_LIBS}
    VERSION ${BUILD_VERSION}
    PRIVATE_DEFINITIONS ${PRIVATE_DEFINITIONS}
    DEFINITIONS ${VIEWER_DEFINITIONS}
    INSTALL_EXPORT_NAME ${PROJECT_NAME}Targets
    INSTALL_NAMESPACE RabbitRemoteControl
    )
