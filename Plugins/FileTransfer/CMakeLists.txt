
project(FileTransfer LANGUAGES CXX
  DESCRIPTION "File transfers")

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
    ChannelSFTP.cpp
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
    ChannelSFTP.h
    )
SET(FileTransfer_UI_FILES
    FrmFileTransfer.ui
    DlgFileTransfer.ui
    )

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
    PRIVATE_LIBS ${FileTransfer_LIBS}
    INSTALL_DIR ${PLUGIN_PATH}
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}
    INSTALL_RPATH ${INSTALL_RPATH}
    VERSION ${RabbitRemoteControl_VERSION}
)
