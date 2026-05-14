# Author: Kang Lin <kl222@126.com>

project(SftpServer)

include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CMakePushCheckState)

set(SftpServer_LIBS Plugin)

find_package(libssh)
if(libssh_FOUND)
    message("libssh version: ${libssh_VERSION}")
    if(libssh_VERSION VERSION_LESS "0.11.0")
        message(WARNING "libssh >= 0.11.0 required (found ${libssh_VERSION})")
        set(BUILD_SFTP_SERVER OFF PARENT_SCOPE)
        return()
    endif()

    if(DEFINED libssh_INCLUDE_DIR)
        set(LIBSSH_INCLUDE_PATH ${libssh_INCLUDE_DIR})
        message("libssh_INCLUDE_DIR: ${libssh_INCLUDE_DIR}")
    elseif(DEFINED LIBSSH_INCLUDE_DIR)
        set(LIBSSH_INCLUDE_PATH ${LIBSSH_INCLUDE_DIR})
    elseif(TARGET ssh)
        get_target_property(LIBSSH_INCLUDE_PATH ssh INTERFACE_INCLUDE_DIRECTORIES)
    else()
        message(WARNING "Cannot determine libssh include directory")
        set(BUILD_SFTP_SERVER OFF PARENT_SCOPE)
        return()
    endif()
    if(EXISTS "${LIBSSH_INCLUDE_PATH}/libssh/server.h")
         message("Find libssh/server.h")
    else()
        message(WARNING "libssh server.h headers not found - disabling SFTP server")
        set(BUILD_SFTP_SERVER OFF PARENT_SCOPE)
        return()
    endif()

    list(APPEND SftpServer_DEFINITIONS HAVE_LIBSSH)
    if(WIN32)
        list(APPEND SftpServer_LIBS ws2_32)
    endif()
    list(APPEND SftpServer_LIBS ssh)

    option(INSTALL_LIBSSH "Install libssh libraries" OFF)
    if(INSTALL_LIBSSH)
        INSTALL_TARGETS(TARGETS ssh)
    endif()
else()
    set(BUILD_SFTP_SERVER OFF PARENT_SCOPE)
    message(WARNING "Don't find libssh. Please download from \"https://www.libssh.org/\" or git clone https://git.libssh.org/projects/libssh.git/")
    return()
endif()

list(APPEND SftpServer_QT_COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${SftpServer_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${SftpServer_QT_COMPONENTS})
        list(APPEND SftpServer_LIBS Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
else()
    message(FATAL_ERROR "Qt${QT_VERSION_MAJOR} don't found")
endif()

SET(SftpServer_SOURCE_FILES
    PluginSftpServer.cpp
    OperateSftpServer.cpp
    BackendSftpServer.cpp
    ParameterSftpServer.cpp
    DlgSettingsSftpServer.cpp
    FrmViewerSftpServer.cpp
    )

SET(SftpServer_HEADER_FILES
    PluginSftpServer.h
    OperateSftpServer.h
    BackendSftpServer.h
    ParameterSftpServer.h
    DlgSettingsSftpServer.h
    FrmViewerSftpServer.h
    )

SET(SftpServer_UI_FILES
    DlgSettingsSftpServer.ui
    FrmViewerSftpServer.ui
    )

ADD_PLUGIN_TARGET(NAME PluginClient${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${SftpServer_SOURCE_FILES} ${SftpServer_HEADER_FILES} ${SftpServer_UI_FILES}
    PRIVATE_LIBS ${SftpServer_LIBS}
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}
    INSTALL_RPATH ${INSTALL_RPATH}
    VERSION ${RabbitRemoteControl_VERSION}
    PRIVATE_DEFINITIONS ${SftpServer_DEFINITIONS} SftpServer_VERSION="${RabbitRemoteControl_VERSION}"
)
