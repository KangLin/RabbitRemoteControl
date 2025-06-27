# Author: Kang Lin <kl222@126.com>

find_package(libssh)

if(NOT libssh_FOUND)
    return()
endif()

set(libssh_LIB ssh)

project(TerminalSsh DESCRIPTION "SSH terminal")

list(APPEND libssh_LIB Plugin)

SET(SOURCE_FILES
    PluginLibSSH.cpp
    OperateSSH.cpp
    DlgSettingsSSH.cpp
    BackendSSH.cpp
    ChannelSSHTerminal.cpp
    )

SET(HEADER_FILES
    PluginLibSSH.h
    OperateSSH.h
    DlgSettingsSSH.h
    BackendSSH.h
    ChannelSSHTerminal.h
    )

SET(UI_FILES
    DlgSettingsSSH.ui
    )

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets)

ADD_PLUGIN_TARGET(NAME PluginClient${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
    PRIVATE_LIBS ${libssh_LIB}
    INSTALL_DIR ${PLUGIN_PATH}
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}
    INSTALL_RPATH ${INSTALL_RPATH}
    VERSION ${RabbitRemoteControl_VERSION}
)
