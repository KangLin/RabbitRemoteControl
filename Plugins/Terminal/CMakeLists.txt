# Author: Kang Lin <kl222@126.com>

project(Terminal
    DESCRIPTION "Terminal plugin")

set(lib_terminal Terminal)

SET(SOURCE_FILES
    PluginTerminal.cpp
    DlgSettingsTerminal.cpp
    Terminal.cpp
    )

SET(HEADER_FILES
    PluginTerminal.h
    DlgSettingsTerminal.h
    Terminal.h
    )

SET(UI_FILES
    DlgSettingsTerminal.ui
    )

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets)

ADD_PLUGIN_TARGET(NAME PluginClient${PROJECT_NAME}
    ISPLUGIN
    VERSION ${RabbitRemoteControl_VERSION}
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
    PRIVATE_LIBS ${lib_terminal}
    INSTALL_DIR ${PLUGIN_PATH}
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}
    )
