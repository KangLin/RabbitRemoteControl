# Author: Kang Lin <kl222@126.com>

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "verbose")

project(Channel LANGUAGES CXX)

list(APPEND CHANNEL_QT_COMPONENTS Core)

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
        set(libdatachannel_lib LibDataChannel::LibDataChannel)
    elseif (TARGET LibDatachannel::datachannel)
        set(libdatachannel_lib LibDatachannel::datachannel)
    else()
        set(libdatachannel_lib LibDataChannel::datachannel)
    endif()
    list(APPEND CHANNEL_SRC_LIBS ${libdatachannel_lib})
    if(UNIX)
        install_targets(TARGETS ${libdatachannel_lib})
    endif()
    list(APPEND CHANNEL_SOURCE_FILES
        ICE/ChannelIce.cpp
        ICE/IceSignal.cpp
        ICE/Ice.cpp
        ICE/ParameterICE.cpp)
    list(APPEND CHANNEL_INSTALL_HEADER_FILES
        ICE/IceSignal.h
        ICE/Ice.h
        ICE/ParameterICE.h)
    list(APPEND CHANNEL_HEADER_FILES
        ICE/ChannelIce.h
        )

    find_package(QXmpp)
    if(QXmpp_FOUND)
        list(APPEND CHANNEL_DEFINITIONS HAVE_QXMPP)
        list(APPEND CHANNEL_QT_COMPONENTS Network Xml)
        list(APPEND CHANNEL_PRIVATE_LIBS QXmpp::QXmpp)
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

    if(WITH_GUI)
        list(APPEND CHANNEL_QT_COMPONENTS Gui Widgets)
        list(APPEND CHANNEL_SOURCE_FILES ICE/FrmParameterICE.cpp)
        list(APPEND CHANNEL_INSTALL_HEADER_FILES ICE/FrmParameterICE.h)
        list(APPEND CHANNEL_UIS ICE/FrmParameterICE.ui)
    endif()
endif()

list(APPEND CHANNEL_HEADER_FILES ${CHANNEL_INSTALL_HEADER_FILES})

list(APPEND CHANNEL_RCC_FILES)

list(APPEND CHANNEL_QT_COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${CHANNEL_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${CHANNEL_QT_COMPONENTS})
        list(APPEND CHANNEL_SRC_LIBS Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
endif()

list(APPEND CHANNEL_SRC_LIBS RabbitCommon)
message("CHANNEL_SRC_LIBS: ${CHANNEL_SRC_LIBS}")
ADD_TARGET(NAME ${PROJECT_NAME}
    SOURCE_FILES ${CHANNEL_SOURCE_FILES} ${CHANNEL_HEADER_FILES}
        ${CHANNEL_UIS} ${CHANNEL_RCC_FILES}
    INCLUDE_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # See: http://www.it1352.com/478094.html
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/ICE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    INSTALL_HEADER_FILES ${CHANNEL_INSTALL_HEADER_FILES}
    LIBS ${CHANNEL_SRC_LIBS}
    PRIVATE_LIBS ${CHANNEL_PRIVATE_LIBS}
    VERSION ${RabbitRemoteControl_VERSION}
    PRIVATE_DEFINITIONS ${PRIVATE_DEFINITIONS}
    DEFINITIONS ${CHANNEL_DEFINITIONS}
    INSTALL_EXPORT_NAME ${PROJECT_NAME}Targets
    INSTALL_NAMESPACE RabbitRemoteControl
    )
