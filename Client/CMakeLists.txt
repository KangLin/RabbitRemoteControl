# Author: Kang Lin <kl222@126.com>

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "verbose")

project(Client
    LANGUAGES CXX
    DESCRIPTION "Client library"
    )

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
    ParameterCompone/ParameterRecord.h
    ParameterCompone/ParameterRecordUI.h
    )

list(APPEND CLIENT_HEADER_FILES
    ${CLIENT_INSTALL_HEADER_FILES}
    FrmViewer.h
    FrmScroll.h
    Connect.h
    ConnectDesktop.h
    ConnectThread.h
    PluginThread.h
    ManageConnect.h
    DlgInputPassword.h
    Hook.h
    FrmParameterClient.h
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
    Connect.cpp
    ConnectDesktop.cpp
    FrmScroll.cpp
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
    ParameterCompone/ParameterRecord.cpp
    ParameterCompone/ParameterRecordUI.cpp
    DlgInputPassword.cpp
    Hook.cpp
    )

set(CLIENT_UI_FILES
    DlgInputPassword.ui
    FrmParameterClient.ui
    ParameterCompone/ParameterUserUI.ui
    ParameterCompone/ParameterNetUI.ui
    ParameterCompone/ParameterRecordUI.ui
    )

if(WIN32)
    list(APPEND CLIENT_HEADER_FILES Windows/HookWindows.h)
    list(APPEND CLIENT_SOURCE_FILES Windows/HookWindows.cpp)
    list(APPEND CLIENT_LIBS Ws2_32)
endif()

list(APPEND CLIENT_LIBS Channel)
if(UNIX AND NOT(ANDROID OR APPLE))
    find_package(X11)
    if(X11_FOUND)
        list(APPEND CLIENT_LIBS X11::X11 X11::xcb X11::xcb_keysyms)
        list(APPEND CLIENT_HEADER_FILES Unix/NativeEventFilterUnix.h)
        list(APPEND CLIENT_SOURCE_FILES Unix/NativeEventFilterUnix.cpp)
    endif()
endif()

SET(CLIENT_QT_COMPONENTS Core Gui Widgets Multimedia MultimediaWidgets)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${CLIENT_QT_COMPONENTS})
message("QT_VERSION:${Qt${QT_VERSION_MAJOR}_VERSION}")
if(Qt${QT_VERSION_MAJOR}_VERSION VERSION_LESS 5.10.0 AND ANDROID)
    message(FATAL_ERROR "Qt must great 5.10.0")
endif()
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${CLIENT_QT_COMPONENTS})
        list(APPEND QT_LIBRARIES Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
else()
    message(FATAL_ERROR "Qt${QT_VERSION_MAJOR} don't found")
endif()
list(APPEND CLIENT_LIBS ${QT_LIBRARIES})
message("CLIENT_LIBS: ${CLIENT_LIBS} ${CLIENT_PRIVATE_LIBS}")
list(APPEND CLIENT_DEFINITIONS
    RabbitRemoteControl_VERSION="${RabbitRemoteControl_VERSION}")
if(QT_VERSION GREATER_EQUAL "6.2.4")
    list(APPEND CLIENT_DEFINITIONS HAVE_QT6_MULTIMEDIA)
endif()
if(QT_VERSION GREATER_EQUAL "6.8.0")
    list(APPEND CLIENT_DEFINITIONS HAVE_QT6_RECORD)
endif()
ADD_TARGET(NAME ${PROJECT_NAME}
    SOURCE_FILES ${CLIENT_SOURCE_FILES} ${CLIENT_HEADER_FILES} ${CLIENT_UI_FILES}
    INCLUDE_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # See: http://www.it1352.com/478094.html
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/ParameterCompone
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        ${CLIENT_INCLUDE_DIRS}
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
