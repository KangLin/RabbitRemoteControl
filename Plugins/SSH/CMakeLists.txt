# Author: Kang Lin <kl222@126.com>

find_package(libssh)

if(NOT libssh_FOUND)
    return()
endif()

set(libssh_LIB ssh)

project(SSH DESCRIPTION "SSH terminal plugin")

list(APPEND libssh_LIB Plugin)

SET(LIBSSH_SOURCE_FILES
    PluginLibSSH.cpp
    OperateSSH.cpp
    DlgSettingsSSH.cpp
    BackendSSH.cpp
    ChannelSSHTerminal.cpp
    ParameterTerminalSSH.cpp
    )

SET(LIBSSH_HEADER_FILES
    PluginLibSSH.h
    OperateSSH.h
    DlgSettingsSSH.h
    BackendSSH.h
    ChannelSSHTerminal.h
    ParameterTerminalSSH.h
    )

SET(LIBSSH_UI_FILES
    DlgSettingsSSH.ui
    )

list(APPEND LIBSSH_QT_COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${LIBSSH_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${LIBSSH_QT_COMPONENTS})
        list(APPEND libssh_LIB Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
else()
    message(FATAL_ERROR "Qt${QT_VERSION_MAJOR} don't found")
endif()

ADD_PLUGIN_TARGET(NAME PluginClient${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${LIBSSH_SOURCE_FILES} ${LIBSSH_HEADER_FILES} ${LIBSSH_UI_FILES} ${RCC_FILES}
    PRIVATE_LIBS ${libssh_LIB}
    INSTALL_DIR ${PLUGIN_PATH}
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}
    INSTALL_RPATH ${INSTALL_RPATH}
    VERSION ${RabbitRemoteControl_VERSION}
)
