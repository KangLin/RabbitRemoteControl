# Author: Kang Lin <kl222@126.com>

project(TemplateServer)

set(TemplateServer_LIBS Plugin)

list(APPEND TemplateServer_QT_COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${TemplateServer_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${TemplateServer_QT_COMPONENTS})
        list(APPEND TemplateServer_LIBS Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
else()
    message(FATAL_ERROR "Qt${QT_VERSION_MAJOR} don't found")
endif()

SET(TemplateServer_SOURCE_FILES
    PluginTemplateServer.cpp
    OperateTemplateServer.cpp
    BackendTemplateServer.cpp
    ParameterTemplateServer.cpp
    DlgSettingsTemplateServer.cpp
    )

SET(TemplateServer_HEADER_FILES
    PluginTemplateServer.h
    OperateTemplateServer.h
    BackendTemplateServer.h
    ParameterTemplateServer.h
    DlgSettingsTemplateServer.h
    )

SET(TemplateServer_UI_FILES
    DlgSettingsTemplateServer.ui
    )

ADD_PLUGIN_TARGET(NAME PluginService${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${TemplateServer_SOURCE_FILES} ${TemplateServer_HEADER_FILES} ${TemplateServer_UI_FILES}
    PRIVATE_LIBS ${TemplateServer_LIBS}
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}
    INSTALL_RPATH ${INSTALL_RPATH}
    VERSION ${RabbitRemoteControl_VERSION}
    PRIVATE_DEFINITIONS TemplateServer_VERSION="${RabbitRemoteControl_VERSION}"
)
