# Author: Kang Lin <kl222@126.com>

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "verbose")

project(Plugin
    LANGUAGES CXX
    DESCRIPTION "Plugin library"
    )

set(PLUGIN_LIBS RabbitCommon)

option(USE_FROM_OPENGL "Use opengl from" OFF)
if(USE_FROM_OPENGL)
    set(PLUGIN_DEFINITIONS USE_FROM_OPENGL)
    list(APPEND PLUGIN_SOURCE_FILES
        FrmViewerOpenGL.cpp)
    list(APPEND PLUGIN_INSTALL_HEADER_FILES
        FrmViewerOpenGL.h)
else()
    list(APPEND PLUGIN_SOURCE_FILES
        FrmViewer.cpp)
endif()

list(APPEND PLUGIN_INSTALL_HEADER_FILES
    Manager.h
    Plugin.h
    Operate.h
    OperateDesktop.h
    Backend.h
    BackendThread.h
    BackendDesktop.h
    Channel/Channel.h
    SSH/Event.h
    DlgUserPassword.h
    Parameter.h
    ParameterUI.h
    ParameterOperate.h
    ParameterCompone/ParameterPlugin.h
    ParameterCompone/ParameterBase.h
    ParameterCompone/ParameterNet.h
    ParameterCompone/ParameterNetUI.h
    ParameterCompone/ParameterUser.h
    ParameterCompone/ParameterUserUI.h
    ParameterCompone/ParameterProxy.h
    ParameterCompone/ParameterProxyUI.h
    ParameterCompone/ParameterRecord.h
    ParameterCompone/ParameterRecordUI.h
    ParameterCompone/ParameterSSH.h
    ParameterCompone/ParameterSSHTunnelUI.h
    )

list(APPEND PLUGIN_HEADER_FILES
    ${PLUGIN_INSTALL_HEADER_FILES}
    FrmViewer.h
    FrmScroll.h
    DlgInputPassword.h
    Hook.h
    ParameterCompone/ParameterPluginUI.h
    )

list(APPEND PLUGIN_SOURCE_FILES
    Manager.cpp
    Plugin.cpp
    Operate.cpp
    OperateDesktop.cpp
    Backend.cpp
    BackendThread.cpp
    BackendDesktop.cpp
    FrmScroll.cpp
    Channel/Channel.cpp
    SSH/Event.cpp
    Parameter.cpp
    ParameterUI.cpp
    ParameterOperate.cpp
    ParameterCompone/ParameterPlugin.cpp
    ParameterCompone/ParameterPluginUI.cpp
    ParameterCompone/ParameterBase.cpp
    ParameterCompone/ParameterNet.cpp
    ParameterCompone/ParameterNetUI.cpp
    ParameterCompone/ParameterUser.cpp
    ParameterCompone/ParameterUserUI.cpp
    ParameterCompone/ParameterProxy.cpp
    ParameterCompone/ParameterProxyUI.cpp
    ParameterCompone/ParameterRecord.cpp
    ParameterCompone/ParameterRecordUI.cpp
    ParameterCompone/ParameterSSH.cpp
    ParameterCompone/ParameterSSHTunnelUI.cpp
    DlgInputPassword.cpp
    DlgUserPassword.cpp
    Hook.cpp
    )

set(PLUGIN_UI_FILES
    DlgInputPassword.ui
    DlgUserPassword.ui
    ParameterCompone/ParameterPluginUI.ui
    ParameterCompone/ParameterUserUI.ui
    ParameterCompone/ParameterNetUI.ui
    ParameterCompone/ParameterRecordUI.ui
    ParameterCompone/ParameterSSHTunnelUI.ui
    )

if(WIN32)
    list(APPEND PLUGIN_HEADER_FILES Windows/HookWindows.h)
    list(APPEND PLUGIN_SOURCE_FILES Windows/HookWindows.cpp)
    list(APPEND PLUGIN_LIBS Ws2_32)
endif()

find_package(LibDataChannel)
if(LibDataChannel_FOUND)
    list(APPEND CHANNEL_DEFINITIONS HAVE_ICE)
    if (TARGET LibDataChannel::LibDataChannel)
        list(APPEND PLUGIN_PRIVATE_LIBS LibDataChannel::LibDataChannel)
    elseif (TARGET LibDatachannel::datachannel)
        list(APPEND PLUGIN_PRIVATE_LIBS LibDatachannel::datachannel)
    else()
        list(APPEND PLUGIN_PRIVATE_LIBS LibDataChannel::datachannel)
    endif()
    list(APPEND PLUGIN_INCLUDE_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/ICE)
    list(APPEND PLUGIN_PRIVATE_LIBS ${libdatachannel_lib})
    list(APPEND PLUGIN_SOURCE_FILES
         Channel/ChannelIce.cpp
         Channel/IceSignal.cpp
         Channel/Ice.cpp
         Channel/ParameterICE.cpp)
    list(APPEND PLUGIN_INSTALL_HEADER_FILES
         Channel/IceSignal.h
         Channel/Ice.h
         Channel/ParameterICE.h)
    list(APPEND PLUGIN_HEADER_FILES
         Channel/ChannelIce.h
        )

    find_package(QXmpp)
    if(QXmpp_FOUND)
        list(APPEND PLUGIN_PRIVATE_DEFINITIONS HAVE_QXMPP)
        list(APPEND PLUGIN_QT_COMPONENTS Xml)
        list(APPEND PLUGIN_PRIVATE_LIBS QXmpp::QXmpp)
        list(APPEND PLUGIN_SOURCE_FILES
             Channel/IceSignalQXmppIq.cpp
             Channel/IceSignalQXmppManager.cpp
             Channel/IceSignalQxmpp.cpp
            )
        list(APPEND PLUGIN_HEADER_FILES
             Channel/IceSignalQXmppIq.h
             Channel/IceSignalQXmppManager.h
             Channel/IceSignalQxmpp.h
            )
    endif()

    if(WITH_GUI)
        list(APPEND PLUGIN_SOURCE_FILES Channel/FrmParameterICE.cpp)
        list(APPEND PLUGIN_INSTALL_HEADER_FILES Channel/FrmParameterICE.h)
        list(APPEND PLUGIN_UI_FILES Channel/FrmParameterICE.ui)
    endif()
endif()

if(UNIX AND (NOT ANDROID))
    find_package(X11)
    if(X11_FOUND)
        list(APPEND PLUGIN_LIBS X11::X11 X11::xcb)
        if(NOT APPLE)
          list(APPEND PLUGIN_PRIVATE_LIBS X11::xcb_keysyms)
          list(APPEND PLUGIN_HEADER_FILES Unix/NativeEventFilterUnix.h)
          list(APPEND PLUGIN_SOURCE_FILES Unix/NativeEventFilterUnix.cpp)
        endif()
    endif()
endif()

find_package(libssh)
if(libssh_FOUND)
    option(WITH_EVENTFD "Enable eventfd" ON)
    if(WITH_EVENTFD)
        check_include_file("sys/eventfd.h" HAVE_EVENTFD_INCLUDE)
        check_function_exists("eventfd" HAVE_EVENTFD)
        if(HAVE_EVENTFD AND HAVE_EVENTFD_INCLUDE)
            list(APPEND PLUGIN_DEFINITIONS HAVE_EVENTFD)
        endif()
    endif(WITH_EVENTFD)

    option(WITH_UNIX_DOMAIN_SOCKET "Enable unix domain socket" ON)
    if(WITH_UNIX_DOMAIN_SOCKET)
        if(WIN32)
            #check_include_file("afunix.h" HAVE_UNIX_DOMAIN_SOCKET)
            check_struct_has_member("struct sockaddr_un" sun_path "afunix.h" HAVE_UNIX_DOMAIN_SOCKET)
        else()
            #check_include_file("sys/un.h" HAVE_UNIX_DOMAIN_SOCKET)
            check_struct_has_member("struct sockaddr_un" sun_path "sys/un.h" HAVE_UNIX_DOMAIN_SOCKET)
        endif()
        if(HAVE_UNIX_DOMAIN_SOCKET)
            list(APPEND PLUGIN_DEFINITIONS HAVE_UNIX_DOMAIN_SOCKET)
        endif()
    endif(WITH_UNIX_DOMAIN_SOCKET)

    list(APPEND PLUGIN_DEFINITIONS HAVE_LIBSSH)
    if(WIN32)
        list(APPEND PLUGIN_PRIVATE_LIBS ws2_32)
    endif()
    list(APPEND PLUGIN_LIBS ssh)
    list(APPEND PLUGIN_INCLUDE_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/SSH)
    list(APPEND PLUGIN_INSTALL_HEADER_FILES
        SSH/ChannelSSH.h
        SSH/ChannelSSHTunnel.h
        SSH/SSHTunnelThread.h
        )
    list(APPEND PLUGIN_HEADER_FILES
        SSH/ChannelSSHTunnelForward.h
        )
    list(APPEND PLUGIN_SOURCE_FILES
        SSH/ChannelSSH.cpp
        SSH/ChannelSSHTunnel.cpp
        SSH/ChannelSSHTunnelForward.cpp
        SSH/SSHTunnelThread.cpp
        )
endif()

find_package(qtermwidget${QT_VERSION_MAJOR})
if(qtermwidget${QT_VERSION_MAJOR}_FOUND)
    list(APPEND PLUGIN_PRIVATE_DEFINITIONS HAVE_QTERMWIDGET)
    list(APPEND PLUGIN_LIBS qtermwidget${QT_VERSION_MAJOR})
    list(APPEND PLUGIN_INCLUDE_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/Terminal)
    list(APPEND PLUGIN_INSTALL_HEADER_FILES
        Terminal/OperateTerminal.h
        Terminal/ParameterTerminalUI.h
        Terminal/ParameterTerminal.h
        Terminal/ParameterTerminalBase.h
    )
    list(APPEND PLUGIN_SOURCE_FILES
        Terminal/OperateTerminal.cpp
        Terminal/ParameterTerminalUI.cpp
        Terminal/ParameterTerminal.cpp
        Terminal/ParameterTerminalBase.cpp
    )
    list(APPEND PLUGIN_UI_FILES
        Terminal/ParameterTerminalUI.ui
    )
endif()

list(APPEND PLUGIN_QT_COMPONENTS Core Gui Widgets Network Multimedia MultimediaWidgets)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${PLUGIN_QT_COMPONENTS})
message("QT_VERSION:${Qt${QT_VERSION_MAJOR}_VERSION}")
if(Qt${QT_VERSION_MAJOR}_VERSION VERSION_LESS 5.10.0 AND ANDROID)
    message(FATAL_ERROR "Qt must great 5.10.0")
endif()
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${PLUGIN_QT_COMPONENTS})
        list(APPEND QT_LIBRARIES Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
else()
    message(FATAL_ERROR "Qt${QT_VERSION_MAJOR} don't found")
endif()
list(APPEND PLUGIN_LIBS ${QT_LIBRARIES})
message("PLUGIN_LIBS: ${PLUGIN_LIBS} ${PLUGIN_PRIVATE_LIBS}")
list(APPEND PLUGIN_DEFINITIONS
    RabbitRemoteControl_VERSION="${RabbitRemoteControl_VERSION}")
if(QT_VERSION GREATER_EQUAL "6.2.4")
    list(APPEND PLUGIN_DEFINITIONS HAVE_QT6_MULTIMEDIA)
endif()
if(QT_VERSION GREATER_EQUAL "6.8.0")
    list(APPEND PLUGIN_DEFINITIONS HAVE_QT6_RECORD)
endif()
ADD_TARGET(NAME ${PROJECT_NAME}
    SOURCE_FILES ${PLUGIN_SOURCE_FILES} ${PLUGIN_HEADER_FILES} ${PLUGIN_UI_FILES}
    INCLUDE_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # See: http://www.it1352.com/478094.html
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/ParameterCompone
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/Channel
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        ${PLUGIN_INCLUDE_DIRS}
    INSTALL_HEADER_FILES ${PLUGIN_INSTALL_HEADER_FILES}
    PRIVATE_LIBS ${PLUGIN_PRIVATE_LIBS}
    LIBS ${PLUGIN_LIBS}
    VERSION ${RabbitRemoteControl_VERSION}
    PRIVATE_DEFINITIONS ${PLUGIN_PRIVATE_DEFINITIONS}
    DEFINITIONS ${PLUGIN_DEFINITIONS}
    INSTALL_EXPORT_NAME ${PROJECT_NAME}Targets
    INSTALL_NAMESPACE RabbitRemoteControl
    )
