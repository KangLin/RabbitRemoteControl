# Author: Kang Lin <kl222@126.com>

project(Player
    DESCRIPTION "Player plugin"
    )

SET(Player_LIBS Client)

SET(Player_SOURCE_FILES
    PluginPlayer.cpp
    ConnecterPlayer.cpp
    ParameterPlayer.cpp
    ConnectPlayer.cpp
    DlgPlayer.cpp
    )

SET(Player_HEADER_FILES
    PluginPlayer.h
    ConnecterPlayer.h
    ParameterPlayer.h
    ConnectPlayer.h
    DlgPlayer.h
    )

SET(Player_UI_FILES
    DlgPlayer.ui
    )

SET(Player_DEFINITIONS
)

set(Player_QT_COMPONENTS Widgets MultimediaWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${Player_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${Player_QT_COMPONENTS})
        list(APPEND Player_LIBS Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
endif()
ADD_PLUGIN_TARGET(NAME PluginClient${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${Player_SOURCE_FILES} ${Player_HEADER_FILES} ${Player_UI_FILES}
    PRIVATE_LIBS ${Player_LIBS}
    PRIVATE_INCLUDE_DIRS ${Player_INCLUDE_DIR}
    PRIVATE_DEFINITIONS ${Player_DEFINITIONS}
       PLayer_VERSION="${RabbitRemoteControl_VERSION}"
    PRIVATE_OPTIONS ${Player_OPTIONS}
    INSTALL_DIR plugins/Client
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins/Client
    INSTALL_RPATH ${INSTALL_RPATH}
    VERSION ${RabbitRemoteControl_VERSION}
    )
