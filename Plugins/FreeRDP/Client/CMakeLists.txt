# Author: Kang Lin <kl222@126.com>

project(FreeRDP
    DESCRIPTION "FreeRDP client plugin"
    )

SET(FreeRDP_LIBS freerdp-client freerdp winpr Plugin)

SET(FreeRDP_SOURCE_FILES
    ParameterFreeRDP.cpp
    PluginFreeRDP.cpp
    BackendFreeRDP.cpp
    OperateFreeRDP.cpp
    DlgSetFreeRDP.cpp
    ConvertKeyCode.cpp
    CursorFreeRDP.cpp
    ClipboardFreeRDP.cpp
    ClipboardMimeData.cpp
    DlgGetUserPasswordFreeRDP.cpp
    DlgDesktopSize.cpp
    )

SET(FreeRDP_HEADER_FILES
    ParameterFreeRDP.h
    PluginFreeRDP.h
    BackendFreeRDP.h
    OperateFreeRDP.h
    DlgSetFreeRDP.h
    ConvertKeyCode.h
    CursorFreeRDP.h
    ClipboardFreeRDP.h
    ClipboardMimeData.h
    DlgGetUserPasswordFreeRDP.h
    DlgDesktopSize.h
    )

SET(FreeRDP_UI_FILES
    DlgSetFreeRDP.ui
    DlgGetUserPasswordFreeRDP.ui
    DlgDesktopSize.ui
    )

find_package(FreeRDP)
if(FreeRDP_VERSION_MAJOR VERSION_GREATER_EQUAL 3)
    list(APPEND FreeRDP_HEADER_FILES ConnectLayer.h ConnectLayerQTcpSocket.h)
    list(APPEND FreeRDP_SOURCE_FILES ConnectLayer.cpp ConnectLayerQTcpSocket.cpp)

    find_package(libssh)
    if(libssh_FOUND)
        list(APPEND FreeRDP_HEADER_FILES ConnectLayerSSHTunnel.h)
        list(APPEND FreeRDP_SOURCE_FILES ConnectLayerSSHTunnel.cpp)
    endif()
endif()

if(WIN32)
    list(APPEND FreeRDP_LIBS Credui.dll)
endif()
if(MINGW)
    set(FreeRDP_OPTIONS -fpermissive)
endif()

set(FreeRDP_QT_COMPONENTS Widgets Multimedia PrintSupport SerialPort)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${FreeRDP_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${FreeRDP_QT_COMPONENTS})
        list(APPEND FreeRDP_LIBS Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
endif()
ADD_PLUGIN_TARGET(NAME PluginClient${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${FreeRDP_SOURCE_FILES} ${FreeRDP_HEADER_FILES} ${FreeRDP_UI_FILES}
    PRIVATE_LIBS ${FreeRDP_LIBS}
    PRIVATE_INCLUDE_DIRS ${FreeRDP_INCLUDE_DIR} ${WinPR_INCLUDE_DIR}
    PRIVATE_DEFINITIONS ${FreeRDP_DEFINITIONS}
       PluginFreeDP_VERSION="${RabbitRemoteControl_VERSION}"
    PRIVATE_OPTIONS ${FreeRDP_OPTIONS}
    INSTALL_DIR ${PLUGIN_PATH}
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}
    INSTALL_RPATH ${INSTALL_RPATH}
    VERSION ${RabbitRemoteControl_VERSION}
    )
