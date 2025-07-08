# Author: Kang Lin <kl222@126.com>

project(Telnet
    DESCRIPTION "Telnet plugin")

set(TELNET_LIBS Plugin)

list(APPEND TELNET_QT_COMPONENTS Core Gui Network Widgets WebSockets)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${TELNET_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${TELNET_QT_COMPONENTS})
        list(APPEND TELNET_LIBS Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
else()
    message(FATAL_ERROR "Qt${QT_VERSION_MAJOR} don't found")
endif()

SET(TELNET_SOURCE_FILES
    PluginTelnet.cpp
    OperateTelnet.cpp
    QTelnet.cpp
    DlgSettingsTelnet.cpp
    ParameterTelnet.cpp
    )

SET(TELNET_HEADER_FILES
    PluginTelnet.h
    OperateTelnet.h
    QTelnet.h
    DlgSettingsTelnet.h
    ParameterTelnet.h
    )

SET(TELNET_UI_FILES
    DlgSettingsTelnet.ui
    )

ADD_PLUGIN_TARGET(NAME PluginClient${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${TELNET_SOURCE_FILES} ${TELNET_HEADER_FILES} ${TELNET_UI_FILES}
    PRIVATE_LIBS ${TELNET_LIBS}
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}
    INSTALL_RPATH ${INSTALL_RPATH}
    VERSION ${RabbitRemoteControl_VERSION}
)
