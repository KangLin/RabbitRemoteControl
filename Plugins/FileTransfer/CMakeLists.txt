
project(FileTransfer LANGUAGES CXX
  DESCRIPTION "File transfers")

SET(FileTransfer_LIBS Plugin)
SET(FileTransfer_SOURCE_FILES
    PluginFileTransfer.cpp
    )
SET(FileTransfer_HEADER_FILES
    PluginFileTransfer.h
    )
SET(FileTransfer_UI_FILES
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

target_sources(PluginClientFileTransfer
  PRIVATE
    OperateFileTransfer.h OperateFileTransfer.cpp
)
