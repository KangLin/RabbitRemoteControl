# Copyright Copyright (c) Kang Lin studio, All Rights Reserved
# Author Kang Lin <kl222@126.com>

project(FileTransfer LANGUAGES CXX
  DESCRIPTION "File transfers plugin")

SET(FileTransfer_LIBS Plugin)
SET(FileTransfer_SOURCE_FILES
    PluginFileTransfer.cpp
    OperateFileTransfer.cpp
    BackendFileTransfer.cpp
    FrmFileTransfer.cpp
    ParameterFileTransfer.cpp
    DlgFileTransfer.cpp
    ListFileModel.cpp
    RemoteFileSystemModel.cpp
    )
SET(FileTransfer_HEADER_FILES
    PluginFileTransfer.h
    OperateFileTransfer.h
    BackendFileTransfer.h
    FrmFileTransfer.h
    ParameterFileTransfer.h
    DlgFileTransfer.h
    ListFileModel.h
    RemoteFileSystemModel.h
    )
SET(FileTransfer_UI_FILES
    FrmFileTransfer.ui
    DlgFileTransfer.ui
    )

find_package(libssh)
if(libssh_FOUND)
    message("libssh version: ${libssh_VERSION}")
    list(APPEND FileTransfer_DEFINED HAVE_LIBSSH)
    if(libssh_VERSION VERSION_LESS "0.11.0")
        message(AUTHOR_WARNING "Don't support libssh file transfer")
    endif()
    list(APPEND FileTransfer_LIBS ssh)
    list(APPEND FileTransfer_SOURCE_FILES
        ChannelSFTP.cpp)
    list(APPEND FileTransfer_HEADER_FILES
        ChannelSFTP.h)
endif()

find_package(CURL)
if(CURL_FOUND)
    list(APPEND FileTransfer_DEFINED HAVE_CURL)
    list(APPEND FileTransfer_LIBS CURL::libcurl)
endif()

# if(MSVC)
#     list(APPEND FileTransfer_DEFINED _CRT_INTERNAL_NONSTDC_NAMES)
# endif()

list(APPEND FileTransfer_QT_COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${FileTransfer_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${FileTransfer_QT_COMPONENTS})
        list(APPEND FileTransfer_LIBS Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
else()
    message(FATAL_ERROR "Qt${QT_VERSION_MAJOR} don't found")
endif()

ADD_PLUGIN_TARGET(NAME PluginClient${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${FileTransfer_SOURCE_FILES} ${FileTransfer_HEADER_FILES} ${FileTransfer_UI_FILES}
    INCLUDE_DIRS ${FileTransfer_INCLUDE}
    PRIVATE_DEFINITIONS ${FileTransfer_DEFINED}
    PRIVATE_LIBS ${FileTransfer_LIBS}
    INSTALL_DIR ${PLUGIN_PATH}
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}
    INSTALL_RPATH ${INSTALL_RPATH}
    VERSION ${RabbitRemoteControl_VERSION}
)
