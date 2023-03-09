# Author: Kang Lin <kl222@126.com>

project(FreeRDP)

SET(FreeRDP_LIBS freerdp-client freerdp Client)
if(UNIX)
    LIST(APPEND FreeRDP_LIBS winpr)
    install_targets(TARGETS ${FreeRDP_LIBS})
endif()

SET(FreeRDP_SOURCE_FILES
    ParameterFreeRDP.cpp
    PluginFreeRDP.cpp
    ConnectFreeRDP.cpp
    ConnecterFreeRDP.cpp
    DlgSetFreeRDP.cpp
    ConvertKeyCode.cpp
    CursorFreeRDP.cpp
    ClipboardFreeRDP.cpp
    ClipboardMimeData.cpp
    DlgGetUserPasswordFreeRDP.cpp
    )

SET(FreeRDP_HEADER_FILES
    ParameterFreeRDP.h
    PluginFreeRDP.h
    ConnectFreeRDP.h
    ConnecterFreeRDP.h
    DlgSetFreeRDP.h
    ConvertKeyCode.h
    CursorFreeRDP.h
    ClipboardFreeRDP.h
    ClipboardMimeData.h
    DlgGetUserPasswordFreeRDP.h
    )

SET(FreeRDP_UI_FILES
    DlgSetFreeRDP.ui
    DlgGetUserPasswordFreeRDP.ui
    )

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
    PRIVATE_DEFINITIONS FreeRDP_VERSION_MAJOR=${FreeRDP_MAJOR} FreeRDP_VERSION_MINOR=${FreeRDP_MINOR}
    PRIVATE_OPTIONS ${FreeRDP_OPTIONS}
    INSTALL_DIR plugins/Client
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins/Client
    VERSION ${RabbitRemoteControl_VERSION}
    )

