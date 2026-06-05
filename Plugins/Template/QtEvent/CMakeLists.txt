# Author: Kang Lin <kl222@126.com>

project(TemplateQtEvent)

set(TemplateQtEvent_LIBS Plugin)

list(APPEND TemplateQtEvent_QT_COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${TemplateQtEvent_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${TemplateQtEvent_QT_COMPONENTS})
        list(APPEND TemplateQtEvent_LIBS Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
else()
    message(FATAL_ERROR "Qt${QT_VERSION_MAJOR} don't found")
endif()

SET(TemplateQtEvent_SOURCE_FILES
    PluginTemplateQtEvent.cpp
    OperateTemplateQtEvent.cpp
    BackendTemplateQtEvent.cpp
    ParameterTemplateQtEvent.cpp
    DlgSettingsTemplateQtEvent.cpp
    FrmViewerTemplateQtEvent.cpp
    )

SET(TemplateQtEvent_HEADER_FILES
    PluginTemplateQtEvent.h
    OperateTemplateQtEvent.h
    BackendTemplateQtEvent.h
    ParameterTemplateQtEvent.h
    DlgSettingsTemplateQtEvent.h
    FrmViewerTemplateQtEvent.h
    )

SET(TemplateQtEvent_UI_FILES
    DlgSettingsTemplateQtEvent.ui
    FrmViewerTemplateQtEvent.ui
    )

ADD_PLUGIN_TARGET(NAME Plugin${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${TemplateQtEvent_SOURCE_FILES} ${TemplateQtEvent_HEADER_FILES} ${TemplateQtEvent_UI_FILES}
    PRIVATE_LIBS ${TemplateQtEvent_LIBS}
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}
    INSTALL_RPATH ${INSTALL_RPATH}
    VERSION ${RabbitRemoteControl_VERSION}
    PRIVATE_DEFINITIONS TemplateQtEvent_VERSION="${RabbitRemoteControl_VERSION}"
)
