# Author: Kang Lin <kl222@126.com>

project(RawStream
    DESCRIPTION "Serial port plugin")

set(RawStreamLibs Plugin)

list(APPEND RawStream_QT_COMPONENTS Core Gui Network Widgets)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${RawStream_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${RawStream_QT_COMPONENTS})
        list(APPEND RawStreamLibs Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
else()
    message(FATAL_ERROR "Qt${QT_VERSION_MAJOR} don't found")
endif()

SET(RawStream_SOURCE_FILES
    PluginRawStream.cpp
    OperateRawStream.cpp
    ParameterRawStream.cpp
    DlgSettingsRawStream.cpp
    )

SET(RawStream_HEADER_FILES
    PluginRawStream.h
    OperateRawStream.h
    ParameterRawStream.h
    DlgSettingsRawStream.h
    )

SET(RawStream_UI_FILES
    DlgSettingsRawStream.ui
    )

ADD_PLUGIN_TARGET(NAME PluginClient${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${RawStream_SOURCE_FILES} ${RawStream_HEADER_FILES} ${RawStream_UI_FILES}
    PRIVATE_LIBS ${RawStreamLibs}
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}
    INSTALL_RPATH ${INSTALL_RPATH}
    VERSION ${RabbitRemoteControl_VERSION}
    PRIVATE_DEFINITIONS PluginRawStream_VERSION="${RabbitRemoteControl_VERSION}"
)
