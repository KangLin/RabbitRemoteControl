# Author: Kang Lin <kl222@126.com>

project(LibVNCServer)

set(_libvncLibs Client)

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
    ConnecterLibVNCServer.cpp
    ConnectLibVNCServer.cpp
    DlgSettingsLibVnc.cpp
    DlgLibVNCServerPassword.cpp
    )

SET(HEADER_FILES
    ParameterLibVNCServer.h
    PluginLibVNCServer.h
    ConnecterLibVNCServer.h
    ConnectLibVNCServer.h
    DlgSettingsLibVnc.h
    DlgLibVNCServerPassword.h
    )

SET(UI_FILES
    DlgSettingsLibVnc.ui
    DlgLibVNCServerPassword.ui
    )

if(_LibvncIncludeDir)
    ADD_PLUGIN_TARGET(NAME PluginClient${PROJECT_NAME}
        ISPLUGIN
        SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
        PRIVATE_LIBS ${_libvncLibs}
        PRIVATE_INCLUDE_DIRS ${_LibvncIncludeDir}
        INSTALL_DIR plugins/Client
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins/Client
        VERSION ${RabbitRemoteControl_VERSION}
        )
else()
    ADD_PLUGIN_TARGET(NAME PluginClient${PROJECT_NAME}
        ISPLUGIN
        SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
        PRIVATE_LIBS ${_libvncLibs}
        INSTALL_DIR plugins/Client
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins/Client
        VERSION ${RabbitRemoteControl_VERSION}
        )
endif()
