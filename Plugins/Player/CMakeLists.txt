# Author: Kang Lin <kl222@126.com>

project(Player
    DESCRIPTION "Player plugin"
    )

SET(Player_LIBS Client)

SET(Player_SOURCE_FILES
    PluginPlayer.cpp
    OperatePlayer.cpp
    ParameterPlayer.cpp
    BackendPlayer.cpp
    DlgPlayer.cpp
    FrmPlayer.cpp
    )

SET(Player_HEADER_FILES
    PluginPlayer.h
    OperatePlayer.h
    ParameterPlayer.h
    BackendPlayer.h
    DlgPlayer.h
    FrmPlayer.h
    )

SET(Player_UI_FILES
    DlgPlayer.ui
    )

option(WITH_QVideoWidget "Use QVideoWidget" ON)
if(WITH_QVideoWidget)
    list(APPEND Player_DEFINITIONS HAVE_QVideoWidget)
endif()

set(Player_QT_COMPONENTS Widgets Multimedia MultimediaWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${Player_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${Player_QT_COMPONENTS})
        list(APPEND Player_LIBS Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
endif()
ADD_PLUGIN_TARGET(NAME Plugin${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${Player_SOURCE_FILES} ${Player_HEADER_FILES} ${Player_UI_FILES}
    PRIVATE_LIBS ${Player_LIBS}
    PRIVATE_INCLUDE_DIRS ${Player_INCLUDE_DIR}
    PRIVATE_DEFINITIONS ${Player_DEFINITIONS}
       PLayer_VERSION="${RabbitRemoteControl_VERSION}"
    PRIVATE_OPTIONS ${Player_OPTIONS}
    INSTALL_DIR plugins
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins
    INSTALL_RPATH ${INSTALL_RPATH}
    VERSION ${RabbitRemoteControl_VERSION}
    )
