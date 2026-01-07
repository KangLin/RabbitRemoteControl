# Author: Kang Lin <kl222@126.com>

project(LibVNCServer
    DESCRIPTION "LibVNCServer client plugin"
    )

set(_libvncLibs Plugin)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Widgets Network)
if(Qt${QT_VERSION_MAJOR}_FOUND)
    list(APPEND _libvncLibs Qt${QT_VERSION_MAJOR}::Network)
endif()

find_package(ZLIB)
if(ZLIB_FOUND)
    list(APPEND _libvncLibs ZLIB::ZLIB)
endif()
find_package(LibVNCServer)
if(LibVNCServer_FOUND)
    list(APPEND _libvncLibs LibVNCServer::vncclient)
    message("LibVNCServer: ${LibVNCServer_FOUND}")
else()
    find_path(LibvncIncludeDir NAMES rfb/rfbclient.h
        HINTS ${LibVNCServer_DIR}
        PATHS ENV LibVNCServer_DIR
        PATH_SUFFIXES include
        )
    message("LibvncIncludeDir:${LibvncIncludeDir}")
    find_library(libvncLibs NAMES vncclient
        HINTS ${LibVNCServer_DIR}
        PATHS ENV LibVNCServer_DIR
        PATH_SUFFIXES lib
        )
    if(NOT (LibvncIncludeDir AND libvncLibs))
        message("Install libvnc and set LibVNCServer_DIR variable to install root directory")
        set(BUILD_LibVNCServer OFF PARENT_SCOPE)
        return()
    endif()
    set(_LibvncIncludeDir ${LibvncIncludeDir})
    list(APPEND _libvncLibs ${libvncLibs})
    mark_as_advanced(LibVNCServer_DIR)
    message("libvnc found: ${_libvncLibs}")
endif()

SET(SOURCE_FILES
    ParameterLibVNCServer.cpp
    PluginLibVNCServer.cpp
    OperateLibVNCServer.cpp
    BackendLibVNCServer.cpp
    DlgSettingsLibVnc.cpp
    )

SET(HEADER_FILES
    ParameterLibVNCServer.h
    PluginLibVNCServer.h
    OperateLibVNCServer.h
    BackendLibVNCServer.h
    DlgSettingsLibVnc.h
    )

SET(UI_FILES
    DlgSettingsLibVnc.ui
    )

if(MSVC)
    # for std::min
    list(APPEND LIBVNCSERVER_PRIVATE_DEFINITIONS NOMINMAX)
endif()

list(APPEND LIBVNCSERVER_PRIVATE_DEFINITIONS
    PluginLibVNCServer_VERSION="${RabbitRemoteControl_VERSION}")

if(_LibvncIncludeDir)
    ADD_PLUGIN_TARGET(NAME PluginClient${PROJECT_NAME}
        ISPLUGIN
        INSTALL_RPATH ${INSTALL_RPATH}
        PRIVATE_DEFINITIONS ${LIBVNCSERVER_PRIVATE_DEFINITIONS}
        SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
        PRIVATE_LIBS ${_libvncLibs}
        PRIVATE_INCLUDE_DIRS ${_LibvncIncludeDir}
        INSTALL_DIR ${PLUGIN_PATH}
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}
        VERSION ${RabbitRemoteControl_VERSION}
        )
else()
    ADD_PLUGIN_TARGET(NAME PluginClient${PROJECT_NAME}
        ISPLUGIN
        INSTALL_RPATH ${INSTALL_RPATH}
        PRIVATE_DEFINITIONS ${LIBVNCSERVER_PRIVATE_DEFINITIONS}
        SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
        PRIVATE_LIBS ${_libvncLibs}
        INSTALL_DIR ${PLUGIN_PATH}
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}
        VERSION ${RabbitRemoteControl_VERSION}
        )
endif()
