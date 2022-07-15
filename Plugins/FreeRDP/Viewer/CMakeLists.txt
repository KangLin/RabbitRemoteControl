# Author: Kang Lin <kl222@126.com>

project(FreeRDP)

SET(SRC_LIBS freerdp-client freerdp Viewer)
if(NOT WIN32)
    LIST(APPEND SRC_LIBS winpr)
endif()

SET(SOURCE_FILES
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

SET(HEADER_FILES
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

SET(UI_FILES
    DlgSetFreeRDP.ui
    DlgGetUserPasswordFreeRDP.ui
    )

if(MINGW)
    set(FreeRDP_OPTIONS -fpermissive)
endif()

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets)

ADD_PLUGIN_TARGET(NAME PluginViewer${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
    PRIVATE_LIBS ${SRC_LIBS}
    PRIVATE_INCLUDE_DIRS ${FreeRDP_INCLUDE_DIR} ${WinPR_INCLUDE_DIR}
    PRIVATE_DEFINITIONS ${RDP_DEFINED}
    PRIVATE_OPTIONS ${FreeRDP_OPTIONS}
    INSTALL_DIR plugins/Viewer
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins/Viewer
    VERSION ${RabbitRemoteControl_VERSION}
    )

