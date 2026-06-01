# Author: Kang Lin <kl222@126.com>

project(FtpServer)

find_package(QFtpServerLib)
if(NOT QFtpServerLib_FOUND)
    message(WARNING "Please set QFtpServerLib_DIR to QFtpServerLib install root or download and build from `https://github.com/KangLin/QFtpServer.git`")
    set(BUILD_FTP_SERVER OFF PARENT_SCOPE)
    return()
    # # Fetch QFtpServer from GitHub
    # include(FetchContent)
    # FetchContent_Declare(
    #     QFtpServerLib
    #     GIT_REPOSITORY https://github.com/KangLin/QFtpServer.git
    #     GIT_TAG master  # or specify a specific version/tag
    # )

    # # Configure QFtpServer build options before making it available
    # set(WITH_APP OFF CACHE BOOL "" FORCE)
    # set(WITH_TEST OFF CACHE BOOL "" FORCE)
    # FetchContent_MakeAvailable(QFtpServerLib)
endif()
message("QFtpServerLib version: ${QFtpServerLib_VERSION}")


set(FtpServer_LIBS Plugin QFtpServerLib)

list(APPEND FtpServer_QT_COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${FtpServer_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${FtpServer_QT_COMPONENTS})
        list(APPEND FtpServer_LIBS Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
else()
    message(FATAL_ERROR "Qt${QT_VERSION_MAJOR} don't found")
endif()

SET(FtpServer_SOURCE_FILES
    PluginFtpServer.cpp
    OperateFtpServer.cpp
    BackendFtpServer.cpp
    ParameterFtpServer.cpp
    DlgSettingsFtpServer.cpp
    )

SET(FtpServer_HEADER_FILES
    PluginFtpServer.h
    OperateFtpServer.h
    BackendFtpServer.h
    ParameterFtpServer.h
    DlgSettingsFtpServer.h
    )

SET(FtpServer_UI_FILES
    DlgSettingsFtpServer.ui
    )

set(FtpServer_QT_COMPONENTS Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${FtpServer_QT_COMPONENTS})
if(Qt${QT_VERSION_MAJOR}_FOUND)
    FOREACH(_COMPONENT ${FtpServer_QT_COMPONENTS})
        list(APPEND FtpServer_LIBS Qt${QT_VERSION_MAJOR}::${_COMPONENT})
    ENDFOREACH()
endif()

ADD_PLUGIN_TARGET(NAME PluginService${PROJECT_NAME}
    ISPLUGIN
    SOURCE_FILES ${FtpServer_SOURCE_FILES} ${FtpServer_HEADER_FILES} ${FtpServer_UI_FILES}
    PRIVATE_LIBS ${FtpServer_LIBS}
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PLUGIN_PATH}
    INSTALL_RPATH ${INSTALL_RPATH}
    VERSION ${RabbitRemoteControl_VERSION}
    PRIVATE_DEFINITIONS FtpServer_VERSION="${RabbitRemoteControl_VERSION}"
        QFtpServerLib_VERSION="${QFtpServerLib_VERSION}"
)

option(INSTALL_QFtpServer "Install QFtpServer libraries" OFF)
if(INSTALL_QFtpServer AND QFtpServerLib_FOUND)
    INSTALL_TARGETS(NAME PluginService${PROJECT_NAME} TARGETS QFtpServerLib)
endif()