# Author: Kang Lin <kl222@126.com>

project(TemplateDesktop)

set(TemplateDesktop_LIBS Plugin)

list(APPEND TemplateDesktop_QT_COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${TemplateDesktop_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${TemplateDesktop_QT_COMPONENTS})
        list(APPEND TemplateDesktop_LIBS Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
else()
    message(FATAL_ERROR "Qt${QT_VERSION_MAJOR} don't found")
endif()

SET(TemplateDesktop_SOURCE_FILES
    PluginTemplateDesktop.cpp
    OperateTemplateDesktop.cpp
    BackendTemplateDesktop.cpp
    ParameterTemplateDesktop.cpp
    DlgSettingsTemplateDesktop.cpp
    )

SET(TemplateDesktop_HEADER_FILES
    PluginTemplateDesktop.h
    OperateTemplateDesktop.h
    BackendTemplateDesktop.h
    ParameterTemplateDesktop.h
    DlgSettingsTemplateDesktop.h
    )

SET(TemplateDesktop_UI_FILES
    DlgSettingsTemplateDesktop.ui
    )

ADD_PLUGIN_TARGET(NAME PluginClient${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${TemplateDesktop_SOURCE_FILES} ${TemplateDesktop_HEADER_FILES} ${TemplateDesktop_UI_FILES}
    PRIVATE_LIBS ${TemplateDesktop_LIBS}
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}
    INSTALL_RPATH ${INSTALL_RPATH}
    VERSION ${RabbitRemoteControl_VERSION}
    PRIVATE_DEFINITIONS TemplateDesktop_VERSION="${RabbitRemoteControl_VERSION}"
)
