# Author: Kang Lin <kl222@126.com>

project(FreeRDPServer)

set(FreeRDPServer_LIBS Plugin freerdp-server freerdp-shadow
    freerdp-shadow-subsystem freerdp-client freerdp)
if(NOT WIN32)
    list(APPEND FreeRDPServer_LIBS winpr)
endif()

list(APPEND FreeRDPServer_QT_COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${FreeRDPServer_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${FreeRDPServer_QT_COMPONENTS})
        list(APPEND FreeRDPServer_LIBS Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
else()
    message(FATAL_ERROR "Qt${QT_VERSION_MAJOR} don't found")
endif()

SET(FreeRDPServer_SOURCE_FILES
    PluginFreeRDPServer.cpp
    OperateFreeRDPServer.cpp
    BackendFreeRDPServer.cpp
    ParameterFreeRDPServer.cpp
    DlgSettingsFreeRDPServer.cpp
    )

SET(FreeRDPServer_HEADER_FILES
    PluginFreeRDPServer.h
    OperateFreeRDPServer.h
    BackendFreeRDPServer.h
    ParameterFreeRDPServer.h
    DlgSettingsFreeRDPServer.h
    )

SET(FreeRDPServer_UI_FILES
    DlgSettingsFreeRDPServer.ui
    )

ADD_PLUGIN_TARGET(NAME PluginService${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${FreeRDPServer_SOURCE_FILES} ${FreeRDPServer_HEADER_FILES} ${FreeRDPServer_UI_FILES}
    PRIVATE_LIBS ${FreeRDPServer_LIBS}
    PRIVATE_INCLUDE_DIRS ${FreeRDP-Shadow_INCLUDE_DIR} ${FreeRDP-Server_INCLUDE_DIR} ${FreeRDP-Client_INCLUDE_DIR} ${FreeRDP_INCLUDE_DIR} ${WinPR_INCLUDE_DIR}
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}
    INSTALL_RPATH ${INSTALL_RPATH}
    VERSION ${RabbitRemoteControl_VERSION}
    PRIVATE_DEFINITIONS FreeRDPServer_VERSION="${RabbitRemoteControl_VERSION}"
)
