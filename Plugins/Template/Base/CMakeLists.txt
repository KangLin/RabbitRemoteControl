# Author: Kang Lin <kl222@126.com>

project(TemplateBase)

set(TemplateBase_LIBS Plugin)

list(APPEND TemplateBase_QT_COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${TemplateBase_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${TemplateBase_QT_COMPONENTS})
        list(APPEND TemplateBase_LIBS Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
else()
    message(FATAL_ERROR "Qt${QT_VERSION_MAJOR} don't found")
endif()

SET(TemplateBase_SOURCE_FILES
    PluginTemplateBase.cpp
    OperateTemplateBase.cpp
    ParameterTemplateBase.cpp
    DlgSettingsTemplateBase.cpp
    )

SET(TemplateBase_HEADER_FILES
    PluginTemplateBase.h
    OperateTemplateBase.h
    ParameterTemplateBase.h
    DlgSettingsTemplateBase.h
    )

SET(TemplateBase_UI_FILES
    DlgSettingsTemplateBase.ui
    )

ADD_PLUGIN_TARGET(NAME PluginClient${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${TemplateBase_SOURCE_FILES} ${TemplateBase_HEADER_FILES} ${TemplateBase_UI_FILES}
    PRIVATE_LIBS ${TemplateBase_LIBS}
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}
    INSTALL_RPATH ${INSTALL_RPATH}
    VERSION ${RabbitRemoteControl_VERSION}
    PRIVATE_DEFINITIONS TemplateBase_VERSION="${RabbitRemoteControl_VERSION}"
)
