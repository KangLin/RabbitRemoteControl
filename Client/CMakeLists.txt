# Author: Kang Lin <kl222@126.com>

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "verbose")

project(Client LANGUAGES CXX)

option(USE_FROM_OPENGL "Use opengl from" OFF)
if(USE_FROM_OPENGL)
    set(CLIENT_DEFINITIONS USE_FROM_OPENGL)
    list(APPEND CLIENT_SOURCE_FILES
        FrmViewerOpenGL.cpp)
    list(APPEND CLIENT_INSTALL_HEADER_FILES
        FrmViewerOpenGL.h)
else()
    list(APPEND CLIENT_SOURCE_FILES
        FrmViewer.cpp)
endif()

list(APPEND CLIENT_INSTALL_HEADER_FILES
    FrmViewer.h
    Client.h
    PluginClient.h
    PluginClientThread.h
    Connecter.h
    ConnecterConnect.h
    ConnecterThread.h
    Parameter.h
    ParameterUI.h
    ParameterConnecter.h
    ParameterClient.h
    ParameterCompone/ParameterBase.h
    ParameterCompone/ParameterNet.h
    ParameterCompone/ParameterNetUI.h
    ParameterCompone/ParameterUser.h
    ParameterCompone/ParameterUserUI.h
    ParameterCompone/ParameterProxy.h
    ParameterCompone/ParameterProxyUI.h
    WakeOnLan/WakeOnLan.h
    WakeOnLan/WakeOnLanQt.h
    WakeOnLan/ParameterWakeOnLan.h
    WakeOnLan/ParameterWakeOnLanUI.h
    )

list(APPEND CLIENT_HEADER_FILES
    ${CLIENT_INSTALL_HEADER_FILES}
    Connect.h
    ConnectDesktop.h
    ConnectThread.h
    ConnectMulti.h
    PluginThread.h
    ManageConnect.h
    DlgInputPassword.h
    Hook.h
    FrmParameterClient.h
    WakeOnLan/ConnectWakeOnLan.h
    )

list(APPEND CLIENT_SOURCE_FILES
    Client.cpp
    PluginClient.cpp
    PluginClientThread.cpp
    PluginThread.cpp
    ManageConnect.cpp
    Connecter.cpp
    ConnecterConnect.cpp
    ConnecterThread.cpp
    ConnectThread.cpp
    ConnectMulti.cpp
    Connect.cpp
    ConnectDesktop.cpp
    Parameter.cpp
    ParameterUI.cpp
    ParameterConnecter.cpp
    ParameterClient.cpp
    FrmParameterClient.cpp
    ParameterCompone/ParameterBase.cpp
    ParameterCompone/ParameterNet.cpp
    ParameterCompone/ParameterNetUI.cpp
    ParameterCompone/ParameterUser.cpp
    ParameterCompone/ParameterUserUI.cpp
    ParameterCompone/ParameterProxy.cpp
    ParameterCompone/ParameterProxyUI.cpp
    WakeOnLan/ParameterWakeOnLan.cpp
    WakeOnLan/ParameterWakeOnLanUI.cpp
    WakeOnLan/WakeOnLan.cpp
    WakeOnLan/WakeOnLanQt.cpp
    WakeOnLan/ConnectWakeOnLan.cpp
    DlgInputPassword.cpp
    Hook.cpp
    )

set(CLIENT_UI_FILES
    DlgInputPassword.ui
    FrmParameterClient.ui
    ParameterCompone/ParameterUserUI.ui
    ParameterCompone/ParameterNetUI.ui
    WakeOnLan/ParameterWakeOnLanUI.ui
    )

if(WIN32)
    list(APPEND CLIENT_HEADER_FILES Windows/HookWindows.h)
    list(APPEND CLIENT_SOURCE_FILES Windows/HookWindows.cpp)
    list(APPEND CLIENT_LIBS Ws2_32)
endif()

list(APPEND CLIENT_LIBS Channel)
if(UNIX)
    find_package(X11)
    if(X11_FOUND)
        list(APPEND CLIENT_LIBS X11::xcb)
        list(APPEND CLIENT_HEADER_FILES Unix/HookUnix.h
            Unix/HookUnixNativeEventFilter.h)
        list(APPEND CLIENT_SOURCE_FILES Unix/HookUnix.cpp
            Unix/HookUnixNativeEventFilter.cpp)
    endif()
endif()

find_package(PcapPlusPlus)
if(PcapPlusPlus_FOUND)
    message(STATUS "Using Pcap++ ${PcapPlusPlus_VERSION}")
    message(STATUS "Include dir: ${PcapPlusPlus_INCLUDE_DIR}")
    list(APPEND CLIENT_DEFINITIONS HAVE_PCAPPLUSPLUS)
    list(APPEND CLIENT_LIBS PcapPlusPlus::Pcap++)
else()
    message("Don't PcapPlusPlus. "
        "Please build and install from git clone https://github.com/seladb/PcapPlusPlus . "
        "And set PcapPlusPlus_DIR to install root directory")
endif()

SET(VIEWER_QT_COMPONENTS Core Gui Widgets)
if(ANDROID)
    SET(VIEWER_QT_COMPONENTS ${VIEWER_QT_COMPONENTS} AndroidExtras)
endif()
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${VIEWER_QT_COMPONENTS})
message("QT_VERSION:${Qt${QT_VERSION_MAJOR}_VERSION}")
if(Qt${QT_VERSION_MAJOR}_VERSION VERSION_LESS 5.10.0 AND ANDROID)
    message(FATAL_ERROR "Qt must great 5.10.0")
endif()
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${VIEWER_QT_COMPONENTS})
        list(APPEND CLIENT_QT_LIBRARIES Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
endif()
list(APPEND CLIENT_LIBS ${CLIENT_QT_LIBRARIES})
message("CLIENT_LIBS: ${CLIENT_LIBS}")
ADD_TARGET(NAME ${PROJECT_NAME}
    SOURCE_FILES ${CLIENT_SOURCE_FILES} ${CLIENT_HEADER_FILES} ${CLIENT_UI_FILES}
    INCLUDE_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # See: http://www.it1352.com/478094.html
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/ParameterCompone
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/WakeOnLan
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    INSTALL_HEADER_FILES ${CLIENT_INSTALL_HEADER_FILES}
    PRIVATE_LIBS ${CLIENT_PRIVATE_LIBS}
    LIBS ${CLIENT_LIBS}
    VERSION ${RabbitRemoteControl_VERSION}
    PRIVATE_DEFINITIONS ${CLIENT_PRIVATE_DEFINITIONS}
    DEFINITIONS ${CLIENT_DEFINITIONS}
    INSTALL_EXPORT_NAME ${PROJECT_NAME}Targets
    INSTALL_NAMESPACE RabbitRemoteControl
    )

option(BUILD_CLIENT_TERMINAL "Build terminal Client" OFF)
if(BUILD_CLIENT_TERMINAL)
    add_subdirectory(Terminal)
endif()
