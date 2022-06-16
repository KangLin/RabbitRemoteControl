# Author: Kang Lin <kl222@126.com>

cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "verbose")

project(Channel LANGUAGES CXX)

list(APPEND CHANNEL_INSTALL_HEADER_FILES
    Channel.h
    )

list(APPEND CHANNEL_SOURCE_FILES
    Channel.cpp
    )

find_package(LibDataChannel)
if(LibDataChannel_FOUND)
    list(APPEND CHANNEL_DEFINITIONS HAVE_ICE)
    if (TARGET LibDataChannel::LibDataChannel)
        list(APPEND CHANNEL_SRC_LIBS LibDataChannel::LibDataChannel)
    elseif (TARGET LibDatachannel::datachannel)
        list(APPEND CHANNEL_SRC_LIBS LibDatachannel::datachannel)
    else()
        list(APPEND CHANNEL_SRC_LIBS LibDataChannel::datachannel)
    endif()
    list(APPEND CHANNEL_SOURCE_FILES
        ICE/DataChannelIce.cpp
        ICE/IceSignal.cpp)
    list(APPEND CHANNEL_INSTALL_HEADER_FILES
        ICE/IceSignal.h)
    list(APPEND CHANNEL_HEADER_FILES
        ICE/DataChannelIce.h
        )
    if(UNIX)
        install_targets(TARGETS LibDataChannel::datachannel)
    endif()
    
    find_package(QXmpp)
    if(QXmpp_FOUND)
        list(APPEND CHANNEL_DEFINITIONS HAVE_QXMPP)
        find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network Xml)
        list(APPEND CHANNEL_SRC_LIBS QXmpp::QXmpp)
        install_targets(TARGETS QXmpp::QXmpp)
        list(APPEND CHANNEL_SOURCE_FILES
            ICE/IceSignalQXmppIq.cpp
            ICE/IceSignalQXmppManager.cpp
            ICE/IceSignalQxmpp.cpp
            )
        list(APPEND CHANNEL_HEADER_FILES
            ICE/IceSignalQXmppIq.h
            ICE/IceSignalQXmppManager.h
            ICE/IceSignalQxmpp.h
            )
    endif()
endif()

list(APPEND CHANNEL_HEADER_FILES ${CHANNEL_INSTALL_HEADER_FILES})

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network)
if(Qt${QT_VERSION_MAJOR}_FOUND)
    list(APPEND CHANNEL_SRC_LIBS Qt${QT_VERSION_MAJOR}::Network)
endif()

list(APPEND CHANNEL_SRC_LIBS RabbitCommon)
message("CHANNEL_SRC_LIBS: ${CHANNEL_SRC_LIBS}")
ADD_TARGET(NAME ${PROJECT_NAME}
    SOURCE_FILES ${CHANNEL_SOURCE_FILES} ${CHANNEL_HEADER_FILES}
    INCLUDE_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # See: http://www.it1352.com/478094.html
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    INSTALL_HEADER_FILES ${CHANNEL_INSTALL_HEADER_FILES}
    LIBS ${CHANNEL_SRC_LIBS}
    VERSION ${BUILD_VERSION}
    PRIVATE_DEFINITIONS ${PRIVATE_DEFINITIONS}
    DEFINITIONS ${CHANNEL_DEFINITIONS}
    INSTALL_EXPORT_NAME ${PROJECT_NAME}Targets
    INSTALL_NAMESPACE RabbitRemoteControl
    )
