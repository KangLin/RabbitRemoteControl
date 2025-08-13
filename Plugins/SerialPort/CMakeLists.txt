# Author: Kang Lin <kl222@126.com>

project(SerialPort
    DESCRIPTION "Serial port plugin")

set(SerialPortLibs Plugin)

list(APPEND SerialPort_QT_COMPONENTS Core Gui Network Widgets SerialPort)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${SerialPort_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${SerialPort_QT_COMPONENTS})
        list(APPEND SerialPortLibs Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
else()
    message(FATAL_ERROR "Qt${QT_VERSION_MAJOR} don't found")
endif()

SET(SerialPort_SOURCE_FILES
    PluginSerialPort.cpp
    OperateSerialPort.cpp
    ParameterSerialPort.cpp
    DlgSettingsSerialPort.cpp
    )

SET(SerialPort_HEADER_FILES
    PluginSerialPort.h
    OperateSerialPort.h
    ParameterSerialPort.h
    DlgSettingsSerialPort.h
    )

SET(SerialPort_UI_FILES
    DlgSettingsSerialPort.ui
    )

ADD_PLUGIN_TARGET(NAME PluginClient${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${SerialPort_SOURCE_FILES} ${SerialPort_HEADER_FILES} ${SerialPort_UI_FILES}
    PRIVATE_LIBS ${SerialPortLibs}
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}
    INSTALL_RPATH ${INSTALL_RPATH}
    VERSION ${RabbitRemoteControl_VERSION}
    PRIVATE_DEFINITIONS PluginSerialPort_VERSION="${RabbitRemoteControl_VERSION}"
)
