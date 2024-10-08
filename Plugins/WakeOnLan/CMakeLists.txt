# Author: Kang Lin <kl222@126.com>

project(WakeOnLan)

SET(WakeOnLan_LIBS Client)

SET(WakeOnLan_SOURCE_FILES
    PluginWakeOnLan.cpp
    ConnecterWakeOnLan.cpp
    FrmWakeOnLan.cpp
    )

SET(WakeOnLan_HEADER_FILES
    PluginWakeOnLan.h
    ConnecterWakeOnLan.h
    FrmWakeOnLan.h
    )

SET(WakeOnLan_UI_FILES
    FrmWakeOnLan.ui
    )

set(WakeOnLan_QT_COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${WakeOnLan_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${WakeOnLan_QT_COMPONENTS})
        list(APPEND WakeOnLan_LIBS Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
endif()
ADD_PLUGIN_TARGET(NAME PluginClient${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${WakeOnLan_SOURCE_FILES} ${WakeOnLan_HEADER_FILES} ${WakeOnLan_UI_FILES}
    PRIVATE_LIBS ${WakeOnLan_LIBS}
    PRIVATE_INCLUDE_DIRS ${WakeOnLan_INCLUDE_DIR} ${WinPR_INCLUDE_DIR}
    PRIVATE_DEFINITIONS ${WakeOnLan_DEFINITIONS}
    PRIVATE_OPTIONS ${WakeOnLan_OPTIONS}
    INSTALL_DIR plugins/Client
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins/Client
    INSTALL_RPATH ${INSTALL_RPATH}
    VERSION ${RabbitRemoteControl_VERSION}
    )
