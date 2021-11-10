# Author: Kang Lin <kl222@126.com>

find_package(WinPR)
message("WinPR_FOUND:${WinPR_FOUND}")

find_package(FreeRDP)
message("FreeRDP_FOUND:${FreeRDP_FOUND}")

find_package(FreeRDP-Client)
message("FreeRDP-Client_FOUND:${FreeRDP-Client_FOUND}")

if(NOT (FreeRDP_FOUND AND WinPR_FOUND AND FreeRDP-Client_FOUND))
    return()
endif()

project(FreeRdp)

SET(SRC_LIBS freerdp-client freerdp Viewer)
if(NOT WIN32)
    LIST(APPEND SRC_LIBS winpr)
endif()

SET(SOURCE_FILES
    PluginFreeRdp.cpp
    ConnectFreeRdp.cpp
    ConnecterFreeRdp.cpp
    DlgSetFreeRdp.cpp
    ConvertKeyCode.cpp
    CursorFreeRdp.cpp
    ClipboardFreeRdp.cpp
    ClipboardMimeData.cpp
    )

SET(HEADER_FILES
    PluginFreeRdp.h
    ConnectFreeRdp.h
    ConnecterFreeRdp.h
    DlgSetFreeRdp.h
    ConvertKeyCode.h
    CursorFreeRdp.h
    ClipboardFreeRdp.h
    ClipboardMimeData.h
    )

SET(UI_FILES
    DlgSetFreeRdp.ui
    )

ADD_PLUGIN_TARGET(NAME PluginViewer${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
    PRIVATE_LIBS ${SRC_LIBS}
    PRIVATE_INCLUDE_DIRS ${FreeRDP_INCLUDE_DIR} ${WinPR_INCLUDE_DIR}
    PRIVATE_DEFINITIONS ${RDP_DEFINED}
    INSTALL_DIR plugins/Viewer
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins/Viewer
    )

