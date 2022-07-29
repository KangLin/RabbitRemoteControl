# Author: Kang Lin <kl222@126.com>

project(Terminal)

set(lib_terminal Terminal)

SET(SOURCE_FILES
    PluginTerminal.cpp
    DlgSettingsTerminal.cpp
    ConnecterPluginTerminal.cpp
    )

SET(HEADER_FILES
    PluginTerminal.h
    DlgSettingsTerminal.h
    ConnecterPluginTerminal.h
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
    INSTALL_DIR plugins/Client
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins/Client
    )
