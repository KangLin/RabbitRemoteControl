# Author: Kang Lin <kl222@126.com>

project(LibVNCServer)

find_package(LibVNCServer)
if(LibVNCServer_FOUND)
    list(APPEND SRC_LIBS LibVNCServer::vncserver)
    message("LibVNCServer: ${LibVNCServer_FOUND}")
else()
    find_path(LibvncIncludeDir NAMES rfb/rfb.h
        HINTS ${LibVNCServer_DIR}
        PATHS ENV LibVNCServer_DIR
        PATH_SUFFIXES include
        )
    message("LibvncIncludeDir:${LibvncIncludeDir}")
    find_library(libvncLibs NAMES vncserver
        HINTS ${LibVNCServer_DIR}
        PATHS ENV LibVNCServer_DIR
        PATH_SUFFIXES lib
        )
    if(NOT (LibvncIncludeDir AND libvncLibs))
        message("Install libvnc and set LibVNCServer_DIR variable to install root directory")
        return()
    endif()
    set(_LibvncIncludeDir ${LibvncIncludeDir})
    list(APPEND SRC_LIBS ${libvncLibs})
    mark_as_advanced(LibVNCServer_DIR)
    message("libvnc found: ${libvncLibs}")
endif()

if(NOT TARGET Service)
    return()
endif()

list(APPEND SRC_LIBS Service)
SET(SOURCE_FILES
    PluginServiceLibVNCServer.cpp
    ServiceLibVNCServer.cpp
    ParameterServiceLibVNC.cpp
    )
SET(HEADER_FILES
    PluginServiceLibVNCServer.h
    ServiceLibVNCServer.h
    ParameterServiceLibVNC.h
    )
SET(UI_FILES
    )

if(WITH_GUI)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets)
    
    list(APPEND SOURCE_FILES FrmParameterServiceLibVNC.cpp FrmParameterServiceLibVNC.ui)
    list(APPEND HEADER_FILES FrmParameterServiceLibVNC.h)
endif()

if(_LibvncIncludeDir)
    ADD_PLUGIN_TARGET(NAME PluginService${PROJECT_NAME}
        ISPLUGIN
        SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
        PRIVATE_LIBS ${SRC_LIBS}
        PRIVATE_INCLUDE_DIRS ${_LibvncIncludeDir}
        INSTALL_DIR plugins/Service
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins/Service
        VERSION ${RabbitRemoteControl_VERSION}
        )
else()
    ADD_PLUGIN_TARGET(NAME PluginService${PROJECT_NAME}
        ISPLUGIN
        SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES} ${RCC_FILES}
        PRIVATE_LIBS ${SRC_LIBS}
        INSTALL_DIR plugins/Service
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins/Service
        VERSION ${RabbitRemoteControl_VERSION}
        )
endif()
