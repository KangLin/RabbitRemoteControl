# Author: Kang Lin <kl222@126.com>

project(ScreenCapture
    DESCRIPTION "Screen capture plugin"
    )

SET(ScreenCapture_LIBS Plugin)

SET(ScreenCapture_SOURCE_FILES
    PluginScreenCapture.cpp
    ScreenCapture.cpp
    DlgCapture.cpp
    ParameterScreenCapture.cpp
    )

SET(ScreenCapture_HEADER_FILES
    PluginScreenCapture.h
    ScreenCapture.h
    DlgCapture.h
    ParameterScreenCapture.h
    )

SET(ScreenCapture_UI_FILES
    DlgCapture.ui
    )

SET(ScreenCapture_DEFINITIONS
)

set(ScreenCapture_QT_COMPONENTS Widgets Multimedia MultimediaWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${ScreenCapture_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${ScreenCapture_QT_COMPONENTS})
        list(APPEND ScreenCapture_LIBS Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
endif()
ADD_PLUGIN_TARGET(NAME Plugin${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${ScreenCapture_SOURCE_FILES} ${ScreenCapture_HEADER_FILES} ${ScreenCapture_UI_FILES}
    PRIVATE_LIBS ${ScreenCapture_LIBS}
    PRIVATE_INCLUDE_DIRS ${ScreenCapture_INCLUDE_DIR}
    PRIVATE_DEFINITIONS ${ScreenCapture_DEFINITIONS}
       ScreenCapture_VERSION="${RabbitRemoteControl_VERSION}"
    PRIVATE_OPTIONS ${ScreenCapture_OPTIONS}
    INSTALL_DIR plugins
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins
    INSTALL_RPATH ${INSTALL_RPATH}
    VERSION ${RabbitRemoteControl_VERSION}
    )
